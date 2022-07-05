#include "apic.hpp"

#include "x64/cpuid.hpp"

#include "asmfunc.h"
#include "logger.hpp"

namespace apic {
  void Initialize() {
    if (cpuid::CheckSupportAPIC()) {
      Log(kError, "APIC is support in this cpu\n");
    } else {
      Log(kError, "APIC is not support in this cpu\n");
      exit(1);
    }
  
    auto msr = ReadMSR(0x1b);
    Log(kError, "MSR: %08lx\n", msr);
    if (!msr & 1 << 11) {
      Log(kError, "This cpu is not support APIC\n");
      exit(1);
    } else {
      Log(kError, "MSR check ok!\n");
    }
  }
}