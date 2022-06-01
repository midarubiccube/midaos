#include "timer.hpp"

#include "acpi/pm_timer.hpp"

#include "apic/apictimer.hpp"

#include "interrupt/interrupt.hpp"
#include "interrupt/interruptVectors.hpp"

#include "task.hpp"

Timer::Timer(unsigned long timeout, uint64_t task_id, int value)
    : timeout_{timeout}, task_id_{task_id}, value_{value} {
}

TimerManager::TimerManager() {
  timers_.push(Timer{std::numeric_limits<unsigned long>::max(), 1, -1});
}

void TimerManager::AddTimer(const Timer& timer) {
  timers_.push(timer);
}

bool TimerManager::Tick() {
  ++tick_;

  bool task_timer_timeout = false;
  while (true) {
    const auto& t = timers_.top();
    if (t.Timeout() > tick_) {
      break;
    }

    if (t.Value() == kTaskTimerValue) {
      task_timer_timeout = true;
      timers_.pop();
      timers_.push(Timer{tick_ + kTaskTimerPeriod, 1, kTaskTimerValue});
      continue;
    }

    Message m{Message::kTimerTimeout};
    m.arg.timer.timeout = t.Timeout();
    m.arg.timer.value = t.Value();
    task_manager->SendMessage(t.TaskID(), m);

    timers_.pop();
  }

  return task_timer_timeout;
}

TimerManager* timer_manager;

void TimerOnInterrupt() {
  const bool task_timer_timeout = timer_manager->Tick();
  Int::NotifyEndOfInterrupt();

  if (task_timer_timeout) {
    task_manager->SwitchTask();
  }
}

void InitializeTimerManager() {
  timer_manager = new TimerManager{};
}
