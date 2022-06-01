#pragma once

#include <cstdint>

#include <vector>

#include "acpi/registers.hpp"

#include "apic/registers.hpp"

namespace apic::ioapic {
  
  class IOAPIC{
  public:
    IOAPIC(acpi::IOAPIC_register* register_base);
    void SetTable(uint16_t IRQ_Number, IOAPIC_REDIR_TBL tbl);

    inline uint8_t GetID() const { return id_register_.bits.APIC_ID; }
    inline uint64_t GetGSI() const { return global_system_interrupt_; }
  private:
    uint8_t id_;
    uint64_t global_system_interrupt_;
    volatile uint32_t& index_register_;
    volatile uint32_t& data_register_;
    volatile uint32_t& EOI_register_;

    IOAPIC_ID id_register_;
    IOAPIC_VER var_register_;
  };

  void Initialize();

  inline std::vector<IOAPIC*> IOAPIC_List;
}