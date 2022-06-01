/**
 * @file acpi.hpp
 *
 * ACPI テーブル定義や操作用プログラムを集めたファイル。
 */

#pragma once

#include <cstdint>
#include <cstddef>

#include <vector>

#include  "acpi/registers.hpp"


namespace acpi {
  void Initialize(const RSDP& rsdp);

  inline XSDT* xsdt_register;

  inline MADT_register* madt_register;

  inline FADT_register* fadt_register;

  inline DSDT_register* dsdt_register;

  inline uint16_t SCI_Interrupt_Pin;
} // namespace acpi
