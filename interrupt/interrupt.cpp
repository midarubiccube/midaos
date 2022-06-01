#include "interrupt/interrupt.hpp"

#include "apic/ioapic.hpp"

#include "ata/ahci.hpp"

#include "interrupt/interruptVectors.hpp"

#include "x64/idt.hpp"
#include "x64/segment.hpp"

#include "asmfunc.h"
#include "logger.hpp"
#include "timer.hpp"
#include "task.hpp"

namespace {
  using namespace Int;

  __attribute__((interrupt))
  void IntHandlerXHCI(InterruptFrame* frame) {
    task_manager->SendMessage(1, Message{Message::kInterruptXHCI});
    NotifyEndOfInterrupt();
  }

  __attribute__((interrupt))
  void IntHandlerLAPICTimer(InterruptFrame* frame) {
    TimerOnInterrupt();
  }

  __attribute__((interrupt))
  void IntHandlerAHCI(InterruptFrame* frame) {
    //ISを設定したときに割込みが入らないように割込みを無効にする
    ata::controller->SetIntteruptFlag(false);
    //コントローラに割り込みが入ったことを伝える
    ata::controller->Interrupt();
    //再度有効にする
    ata::controller->SetIntteruptFlag(true);
    NotifyEndOfInterrupt();
  }

  __attribute__((interrupt))
  void IntHandlerKeyboard(InterruptFrame* frame) {
    Log(kError, "Keyboard interrupt\n");
  }
}

namespace Int {
  void InitializeInterrupt() {
    SetIDTEntry(idt[InterruptVector::kXHCI],
                MakeIDTAttr(DescriptorType::kInterruptGate, 0),
                reinterpret_cast<uint64_t>(IntHandlerXHCI),
                kKernelCS);
                
    SetIDTEntry(idt[InterruptVector::kLAPICTimer],
                MakeIDTAttr(DescriptorType::kInterruptGate, 0),
                reinterpret_cast<uint64_t>(IntHandlerLAPICTimer),
                kKernelCS);

    SetIDTEntry(idt[InterruptVector::kAHCI],
                MakeIDTAttr(DescriptorType::kInterruptGate, 0),
                reinterpret_cast<uint64_t>(IntHandlerAHCI),
                kKernelCS);

    SetIDTEntry(idt[InterruptVector::kKeyboardController],
                MakeIDTAttr(DescriptorType::kInterruptGate, 0),
                reinterpret_cast<uint64_t>(IntHandlerKeyboard),
                kKernelCS);
                
    LoadIDT(sizeof(idt) - 1, reinterpret_cast<uintptr_t>(&idt[0]));
  }

  void NotifyEndOfInterrupt() {
    volatile auto end_of_interrupt = reinterpret_cast<uint32_t*>(0xfee000b0);
    *end_of_interrupt = 0;
  }
}
