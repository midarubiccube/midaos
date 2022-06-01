#pragma once

#include <cstdint>
#include <vector>

#include "interrupt/interruptflag.hpp"

#include "error.hpp"


namespace pci {
  /** @brief CONFIG_ADDRESS レジスタの IO ポートアドレス */
  const uint16_t kConfigAddress = 0x0cf8;
  /** @brief CONFIG_DATA レジスタの IO ポートアドレス */
  const uint16_t kConfigData = 0x0cfc;

  const uint8_t kCapabilityMSI = 0x05;
  const uint8_t kCapabilityMSIX = 0x11;

  union MSI_Header {
    uint32_t data;
    struct {
      uint32_t cap_id : 8;
      uint32_t next_ptr : 8;
      uint32_t msi_enable : 1;
      uint32_t multi_msg_capable : 3;
      uint32_t multi_msg_enable : 3;
      uint32_t addr_64_capable : 1;
      uint32_t per_vector_mask_capable : 1;
      uint32_t : 7;
    } __attribute__((packed)) bits;
  } __attribute__((packed));

  class PCIDevice {
  public:
    uint8_t bus, device, function;

    PCIDevice(uint8_t bus, uint8_t device, uint8_t function);

    bool Match(uint8_t b) { return b == base_; }
    /** @brief ベースクラスとサブクラスが等しい場合に真を返す */
    bool Match(uint8_t b, uint8_t s) { return Match(b) && s == sub_; }
    /** @brief ベース，サブ，インターフェースが等しい場合に真を返す */
    bool Match(uint8_t b, uint8_t s, uint8_t i) {
      return Match(b, s) && i == interface_;
    }

    bool IsSingleFunctionDevice();

    uint32_t ReadConfReg(uint8_t reg_addr);
    void WriteConfReg(uint8_t reg_addr, uint32_t value);
    
    WithError<uint64_t> ReadBar(uint8_t bar_index);
    uint8_t Find_CapAddress(uint8_t cap_id);
    void ConfigureMSI(uint8_t apic_id,
      interrupt::TriggerMode trigger_mode, interrupt::DeliveryMode delivery_mode,
      uint8_t vector, unsigned int num_vector_exponent);
    
    inline uint16_t Vendor_id() const { return vendor_id_; }
    inline uint8_t header_type() const { return header_type_; }
    inline uint8_t Base() const { return base_; }
    inline uint8_t Sub() const { return sub_; }
    inline uint8_t Interface() const { return interface_; }
  private:
    uint32_t io_base_adress;
    uint16_t vendor_id_;
    uint8_t  header_type_, base_, sub_, interface_;
    uint16_t pci_device_id_;
  };

  inline std::vector<PCIDevice*> devices;
  
  bool IsConnected(uint8_t bus, uint8_t device, uint8_t function);

  PCIDevice* FindDevice(uint8_t base);
  PCIDevice* FindDevice(uint8_t base, uint8_t sub);
  PCIDevice* FindDevice(uint8_t base, uint8_t sub, uint8_t interface);
}

void InitializePCI();
