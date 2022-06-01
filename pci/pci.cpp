#include "pci.hpp"

#include "asmfunc.h"
#include "logger.hpp"

namespace {
  using namespace pci;
   void WriteAddress(uint32_t address) {
    IoOut32(kConfigAddress, address);
  }

  void WriteData(uint32_t value) {
    IoOut32(kConfigData, value);
  }

  uint32_t ReadData() {
    return IoIn32(kConfigData);
  }
  void ScanBus(uint8_t bus);

  void ScanFunction(uint8_t bus, uint8_t device, uint8_t function) {
    //ビープ領域に確保する
    auto pci_device = new PCIDevice{bus, device, function};

    devices.push_back(pci_device);
    if (pci_device->Match(0x06u, 0x04u)) {
      uint8_t bus_number = (pci_device->ReadConfReg(0x18) >> 8) & 0xffu;
      ScanBus(bus_number);
    }
  }

  void ScanDevice(uint8_t bus, uint8_t device) {
    ScanFunction(bus, device, 0);
    PCIDevice function0{bus, device, 0};

    if (function0.IsSingleFunctionDevice()){
      for (uint8_t function = 1; function < 8; ++function) {
        if (IsConnected(bus, device, function)) {
          continue;
        }

        ScanFunction(bus, device, function);
      }
    }
  }

  void ScanBus(uint8_t bus) {
    for (uint8_t device = 0; device < 32; ++device) {
      
      if (IsConnected(bus, device, 0)) {
        continue;
      }
      
      ScanDevice(bus, device);
    }
  }

  void ScanAllBus() {
    //どのバスが存在するか確認して存在しているバスをスキャンする
    PCIDevice host_brige{0, 0, 0};
    
    //バスが複数あるか確認する
    if (host_brige.IsSingleFunctionDevice()){
      return ScanBus(0);
    } else {
      for (uint8_t fuction = 0; fuction < 8; ++fuction) {
        if (IsConnected(0, 0, fuction)) {
          continue;
        }
        ScanBus(fuction);
      }
    }
  }
}

namespace pci {
  PCIDevice::PCIDevice(uint8_t bus, uint8_t device, uint8_t function)
    :
    bus{bus},
    device{device},
    function{function}
  {
    auto shl = [](uint32_t x, unsigned int bits){
      return x << bits;
    };

    io_base_adress = shl(1, 31)
                | shl(bus, 16)
                | shl(device, 11)
                | shl(function, 8);
                
    vendor_id_ = ReadConfReg(0x00) & 0xffffu;
    header_type_ = (ReadConfReg(0x0c) >> 16) & 0xffu;

    auto reg = ReadConfReg(0x08);
    base_       = (reg >> 24) & 0xffu;
    sub_        = (reg >> 16) & 0xffu;
    interface_  = (reg >> 8) & 0xffu;
  }

  uint32_t PCIDevice::ReadConfReg(uint8_t reg_addr) {
    WriteAddress(io_base_adress | (reg_addr & 0xfc));
    return ReadData();
  }

  void PCIDevice::WriteConfReg(uint8_t reg_addr, uint32_t value) {
    WriteAddress(io_base_adress | (reg_addr & 0xfc));
    WriteData(value);
  }

  bool PCIDevice::IsSingleFunctionDevice() {
    return (header_type_ & 0x80u) == 0; 
  }

  uint8_t PCIDevice::Find_CapAddress(uint8_t cap_id) {
    uint8_t cap_addr = ReadConfReg(0x34) & 0xffu;
    
    while (cap_addr != 0) {
      auto header = ReadConfReg(cap_addr);
      if ((header & 0xffu) == cap_id) return cap_addr;
      cap_addr = (header >> 8) & 0xffu;
    }
    return 0;
  }

  WithError<uint64_t> PCIDevice::ReadBar(uint8_t bar_index) {
    if (bar_index >= 6) {
      return {0, MAKE_ERROR(Error::kIndexOutOfRange)};
    }

    const auto addr = 0x10 + 4 * bar_index;
    const auto bar = ReadConfReg(addr);

    // 32 bit address
    if ((bar & 4u) == 0) {
      return {bar, MAKE_ERROR(Error::kSuccess)};
    }

    // 64 bit address
    if (bar_index >= 5) {
      return {0, MAKE_ERROR(Error::kIndexOutOfRange)};
    }

    const auto bar_upper = ReadConfReg(addr + 4);
    return {
      bar | (static_cast<uint64_t>(bar_upper) << 32),
      MAKE_ERROR(Error::kSuccess)
    };
  }

  void PCIDevice::ConfigureMSI(uint8_t apic_id,
      interrupt::TriggerMode trigger_mode, interrupt::DeliveryMode delivery_mode,
      uint8_t vector, unsigned int num_vector_exponent) {

    uint32_t msg_addr = 0xfee00000u | (apic_id << 12);
    uint32_t msg_data = (static_cast<uint32_t>(delivery_mode) << 8) | vector;

    if (trigger_mode == interrupt::TriggerMode::kLevel) {
      msg_data |= 0xc000;
    }
    auto msi_cap_addr = Find_CapAddress(kCapabilityMSI);
    

    MSI_Header header{};
    header.data = ReadConfReg(msi_cap_addr);

    if (header.bits.multi_msg_capable <= num_vector_exponent) {
      header.bits.multi_msg_enable =
        header.bits.multi_msg_capable;
    } else {
      header.bits.multi_msg_enable = num_vector_exponent;
    }
    header.bits.msi_enable = 1;

    WriteConfReg(msi_cap_addr, header.data);
    WriteConfReg(msi_cap_addr + 4, msg_addr);

    uint8_t msg_data_addr = msi_cap_addr + 8;
    if (header.bits.addr_64_capable) {
      WriteConfReg(msi_cap_addr + 8, 0);
      msg_data_addr = msi_cap_addr + 12;
    }

    WriteConfReg(msg_data_addr, msg_data);

    if (header.bits.per_vector_mask_capable) {
      WriteConfReg(msg_data_addr + 4, 0);
      WriteConfReg(msg_data_addr + 8, 0);
    }
  }

  bool IsConnected(uint8_t bus, uint8_t device, uint8_t function) {
    auto shl = [](uint32_t x, unsigned int bits){
      return x << bits;
    };
    
    WriteAddress(
      shl(1, 31)|shl(bus, 16)|
      shl(device, 11)|shl(function, 8)
    );

    return (ReadData() & 0xffffu) == 0xffffu;
  }

  PCIDevice* FindDevice(uint8_t base) {
    for (auto dev : devices) {
      if (dev->Match(base)) return dev;
    }
    return nullptr;
  }

  PCIDevice* FindDevice(uint8_t base, uint8_t sub) {
    for (auto dev : devices) {
      if (dev->Match(base, sub)) return dev;
    }
    return nullptr;
  }
  PCIDevice* FindDevice(uint8_t base, uint8_t sub, uint8_t interface) {
    for (auto dev : devices) {
      if (dev->Match(base, sub, interface)) return dev;
    }
    return nullptr;
  }
}

void InitializePCI() {
    ScanAllBus();
    for (auto dev : devices) {
      Log(kError, "%02x:%02x.%d vend=%04x head=%02x class=%02x.%02x.%02x\n",
          dev->bus, dev->device, dev->function, dev->Vendor_id(), dev->header_type(),
          dev->Base(), dev->Sub(), dev->Interface());
    }
}