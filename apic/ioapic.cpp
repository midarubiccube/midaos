#include "apic/ioapic.hpp"

#include "acpi/acpi.hpp"

#include "logger.hpp"

namespace apic::ioapic {
  IOAPIC::IOAPIC(acpi::IOAPIC_register* register_base)
    :
    id_{register_base->IOAPIC_ID},
    global_system_interrupt_{register_base->GSI},
    index_register_{*reinterpret_cast<uint32_t*>(register_base->IOAPIC_addr)},
    data_register_{*reinterpret_cast<uint32_t*>(register_base->IOAPIC_addr + 0x10)},
    EOI_register_{*reinterpret_cast<uint32_t*>(register_base->IOAPIC_addr + 0x40)}
    {
      Log(kError, "I/O APIC info ID %d, RegisterAddress %p, GSI %d\n", id_, &EOI_register_, global_system_interrupt_);
      index_register_ = 0x0;
      id_register_.data = data_register_;
      Log(kError, "ID %d\n", id_register_.bits.APIC_ID);
      index_register_ = 0x1;
      var_register_.data = data_register_;
      Log(kError, "VER: %d\n", var_register_.bits.MRE);
    }
  
  void IOAPIC::SetTable(uint16_t IRQ_Number, IOAPIC_REDIR_TBL tbl) {
    auto offset = (IRQ_Number * 0x2) + 0x10;
    index_register_ = offset;
    data_register_ = tbl.data[0];
    index_register_ = offset + 1;
    data_register_ = tbl.data[1];
  }

  void Initialize() {
    Log(kError, "8259 support %s\n", acpi::madt_register->flags ? "true" : "false");
    for (auto i = 0; i < acpi::madt_register->Count(); ++i) {
      auto entry = acpi::madt_register->Check(i);
      if(entry->type == static_cast<uint8_t>(acpi::ICSType::IO_APIC)) {
        IOAPIC_List.push_back(
          new IOAPIC{
            reinterpret_cast<acpi::IOAPIC_register*>(entry)
          }
        );
      } else if(entry->type == static_cast<uint8_t>(acpi::ICSType::Interrupt_Source_Override)) {
        auto registers = reinterpret_cast<acpi::InterruptSourceOverride_register*>(entry);
        Log(kError, "ISA bus: %d, GSI: %d, Trigger_Mode: %d\n", registers->source, registers->GSI, registers->flag.Trigger_Mode);
      }
    }
  }
}