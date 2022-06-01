#include "acpi/pm_timer.hpp"

#include "acpi/acpi.hpp"

#include "asmfunc.h"

namespace acpi {
  void WaitMilliseconds(unsigned long msec) {
    const bool pm_timer_32 = (fadt_register->Flags >> 8) & 1;
    const uint32_t start = IoIn32(fadt_register->PM_TMR_BLK);
    uint32_t end = start + kPMTimerFreq * msec / 1000;
    if (!pm_timer_32) {
      end &= 0x00ffffffu;
    }

    if (end < start) { // overflow
      while (IoIn32(fadt_register->PM_TMR_BLK) >= start);
    }
    while (IoIn32(fadt_register->PM_TMR_BLK) < end);
  }
}
