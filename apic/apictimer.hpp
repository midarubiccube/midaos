#pragma once

#include <cstdint>

namespace apic::timer {
  void Initialize();
  void StartLAPICTimer();
  uint32_t LAPICTimerElapsed();
  void StopLAPICTimer();

  extern unsigned long lapic_timer_freq;
  const int kTimerFreq = 100;
  extern unsigned long lapic_timer_freq;
}