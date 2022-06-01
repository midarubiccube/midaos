#pragma once

class InterruptVector {
 public:
  enum Number {
    kXHCI = 0x40,
    kLAPICTimer = 0x41,
    kAHCI = 0x42,
    kKeyboardController = 0x43
  };
};

