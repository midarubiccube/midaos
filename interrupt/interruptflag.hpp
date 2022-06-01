#pragma once

namespace interrupt {
  enum class TriggerMode {
    kEdge = 0,
    kLevel = 1
  };

  enum class DeliveryMode {
    kFixed          = 0b000,
    kLowestPriority = 0b001,
    kSMI            = 0b010,
    kNMI            = 0b100,
    kINIT           = 0b101,
    kExtINT         = 0b111,
  };
}