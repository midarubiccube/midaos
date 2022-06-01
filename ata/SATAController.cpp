#include "ata/SATAcontroller.hpp"

#include "ata/port.hpp"
#include "ata/registers.hpp"

#include "logger.hpp"
#include "message.hpp"
#include "memory_manager.hpp"

namespace{
  using namespace ata;
  uint16_t dev_info_temp[256];

  void Log_ptr(void* ptr){
    uint8_t* p = reinterpret_cast<uint8_t*>(ptr);
    Log(kError, "Volume Image:\n");
    for (int i = 0; i < 16; ++i) {
      Log(kError,"%04x:", i * 16);
      for (int j = 0; j < 8; ++j) {
        Log(kError," %02lx", *p);
        ++p;
      }
      Log(kError," ");
      for (int j = 0; j < 8; ++j) {
        Log(kError," %02lx", *p);
        ++p;
      }
      Log(kError," ");
      p -= 16;
      Log(kError, "%.16s", p);
      p += 16;
      Log(kError, "\n");
    }
  }
}

namespace ata{
  SATAController::SATAController(PortController* pc)
    :
    dev_info{},
    register_{&port_register_base_[pc->GetPortNumber()]},
    command_list{&command_heder_list_base_[pc->GetPortNumber()]},
    Receive_FIS_{&rsv_fis_base_[pc->GetPortNumber()]},
    pc_{pc},
    Port_Number_{pc->GetPortNumber()}
  {
    memset(&dev_info_temp, 0, 512);
    SendH2DFIS(
      CommandType::IDENTIFY_DEVICE,
      0,
      reinterpret_cast<uintptr_t>(&dev_info_temp),
      1,
      false
    );
  }

  void SATAController::SendH2DFIS(
      CommandType type, uintptr_t lba,
      uintptr_t dba, uint16_t count, bool w)
  {
    auto slot = FindFreeSlot();
    
    auto table_ptr = reinterpret_cast<Command_Table*>(memory_manager->Allocate(2).value.Frame());
    auto h2d_fis = reinterpret_cast<ata::Register_H2D*>(table_ptr);
    
    h2d_fis->fis_type = FIS_Type::H2D_register_num;
    h2d_fis->c = 1;

    h2d_fis->command = static_cast<uint8_t>(type);
    h2d_fis->device = 1 << 6;

    h2d_fis->SetLBA(reinterpret_cast<uint64_t>(lba));
    h2d_fis->SetCount(count);

    table_ptr->Build_Command_table(
      count,
      reinterpret_cast<void*>(dba)
    );

    command_list->Build_Header(
      table_ptr, slot,
      ((count - 1) >> 4) + 1,
      sizeof(Register_H2D)/sizeof(uint32_t),
      w
    );
  
    register_->PxCI.SetCI(slot);
    command_que.push_back({slot, type});
  }

  void SATAController::InterruptMessage(Interrupt_Type type) {
    switch (type) {
    case Interrupt_Type::DHRS:
      InterruptD2H();
      break;
    default:
      break;
    }
  }

  void SATAController::InterruptD2H() {
    auto cmd = command_que.front();
    command_que.pop_front();

    switch (cmd.type) {
    case CommandType::IDENTIFY_DEVICE:
      dev_info.replace(reinterpret_cast<uint8_t*>(dev_info_temp));
      Model_number = dev_info.bits.Model_number;
      Firmware_revision = dev_info.bits.Firmware_revision;
      Serial_Number = dev_info.bits.Serial_Number;
      Log(kError, "Model Number: %.40s\n", dev_info.bits.Model_number);
      Log(kError, "Firmware_revision: %.8s\n", dev_info.bits.Firmware_revision);
      Log(kError, "Serial_Number: %.20s\n", dev_info.bits.Serial_Number);
      break;
    case CommandType::READ_DMA_EXT:
      Log_ptr(dev_info_temp);
      break;
    }
  }

  uint8_t SATAController::FindFreeSlot() {
    uint32_t slots = (register_->PxCI.CI | register_->PxSACT.DS);
    for (int i = 0; i < ghc_->cap.bits.NCS; ++i) {
      if((slots & 1) == 0) return i;
      slots >>= 1;
    }
    return 0;
  }
}