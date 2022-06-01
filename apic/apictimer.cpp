#include "apic/apictimer.hpp"

#include "acpi/pm_timer.hpp"

#include "interrupt/interruptVectors.hpp"

namespace {
  const uint32_t kCountMax = 0xffffffffu;
  volatile uint32_t& lvt_timer = *reinterpret_cast<uint32_t*>(0xfee00320);
  volatile uint32_t& initial_count = *reinterpret_cast<uint32_t*>(0xfee00380);
  volatile uint32_t& current_count = *reinterpret_cast<uint32_t*>(0xfee00390);
  volatile uint32_t& divide_config = *reinterpret_cast<uint32_t*>(0xfee003e0);
}


namespace apic {
  void InitializeLAPICTimer() {

  divide_config = 0b1011; // divide 1:1
  lvt_timer = 0b001 << 16; // masked, one-shot

  unsigned long lapic_timer_freq;

  StartLAPICTimer();
  acpi::WaitMilliseconds(100);
  const auto elapsed = LAPICTimerElapsed();
  StopLAPICTimer();

  lapic_timer_freq = static_cast<unsigned long>(elapsed) * 10;

  divide_config = 0b1011; // divide 1:1
  lvt_timer = (0b010 << 16) | InterruptVector::kLAPICTimer; // not-masked, periodic
  initial_count = lapic_timer_freq / kTimerFreq;
  }

  void StartLAPICTimer() {
  initial_count = kCountMax;
}

  uint32_t LAPICTimerElapsed() {
    return kCountMax - current_count;
  }

  void StopLAPICTimer() {
    initial_count = 0;
  }
}