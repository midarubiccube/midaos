#pragma once

#include <cstdint>

namespace acpi {
  const int kPMTimerFreq = 3579545;

  void WaitMilliseconds(unsigned long msec);
}