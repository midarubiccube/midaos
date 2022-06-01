#pragma once

#include <cstdint>
#include <queue>
#include <vector>
#include <limits>

#include "message.hpp"


class Timer {
 public:
  Timer(unsigned long timeout, uint64_t task_id, int value);
  unsigned long Timeout() const { return timeout_; }
  int Value() const { return value_; }
  uint64_t TaskID() const { return task_id_; }
 private:
  unsigned long timeout_;
  uint64_t task_id_;
  int value_;
};

/** @brief タイマー優先度を比較する。タイムアウトが遠いほど優先度低。 */
inline bool operator<(const Timer& lhs, const Timer& rhs) {
  return lhs.Timeout() > rhs.Timeout();
}

class TimerManager {
 public:
  TimerManager();
  void AddTimer(const Timer& timer);
  bool Tick();
  unsigned long CurrentTick() const { return tick_; }

 private:
  volatile unsigned long tick_{0};
  std::priority_queue<Timer> timers_{};
};

extern TimerManager* timer_manager;
extern unsigned long lapic_timer_freq;
const int kTimerFreq = 100;

const int kTaskTimerPeriod = static_cast<int>(kTimerFreq * 0.02);
const int kTaskTimerValue = std::numeric_limits<int>::min();

void TimerOnInterrupt();

void InitializeTimerManager();
