#include "acpi/acpi.hpp"

#include <cstdlib>

#include "asmfunc.h"
#include "logger.hpp"

namespace acpi {
  
  void Initialize(const RSDP& rsdp) {
    if (!rsdp.IsValid()) {
      Log(kError, "RSDP is not valid\n");
      exit(1);
    }


    xsdt_register = reinterpret_cast<XSDT*>(rsdp.xsdt_address);

    if (!xsdt_register->header.IsValid("XSDT")) {
      Log(kError, "XSDT is not valid\n");
      exit(1);
    }


    fadt_register = reinterpret_cast<FADT_register*>(xsdt_register->FindTable("FACP"));

    if (!fadt_register->header.IsValid("FACP")) {
      Log(kError, "FACP is not valid");
      exit(1);
    }

    SCI_Interrupt_Pin = fadt_register->SCI_INT;

    Log(kError, "Count: %d\n", xsdt_register->Count());
    madt_register = reinterpret_cast<MADT_register*>(xsdt_register->FindTable("APIC"));

    if (!madt_register->header.IsValid("APIC")) {
      Log(kError, "MADT is not valid");
      exit(1);
    }

    madt_register->FindTable();
  
    xsdt_register->CheckTable(); 

    Log(kError, "SCI interupt pin %04lx\n", SCI_Interrupt_Pin);

    dsdt_register = reinterpret_cast<DSDT_register*>(fadt_register->DSDT);

    if (!dsdt_register->header.IsValid("DSDT")) {
      Log(kError, "DSDT is not valid");
      exit(1);
    }

    Log(kError, "DSDT pointer %p, size %p\n", dsdt_register, dsdt_register->header.length);

    /*if (!dsdt_register->header.IsValid("DSDT")) {
      Log(kError, "DSDT is not valid");
      exit(1);
    }*/

    IoOut32(fadt_register->SMI_CMD, fadt_register->ACPI_ENABLE);

    Log(kError, "SMI_CMD: %08lx, ACPI_ENABLE: %08lx\n", fadt_register->SMI_CMD, fadt_register->ACPI_ENABLE);
  }
}