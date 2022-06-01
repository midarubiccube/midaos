#pragma once

#include <cstdint>

namespace apic {
  void InitializeLAPICTimer();
  void StartLAPICTimer();
  uint32_t LAPICTimerElapsed();
  void StopLAPICTimer();

  extern unsigned long lapic_timer_freq;
  const int kTimerFreq = 100;
  extern unsigned long lapic_timer_freq;
}