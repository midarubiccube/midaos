#pragma once

#include <cstdint>

#define COM1_PORT 0x3F8

namespace SerialPort {
  void Initialize();
  void PutAscii(const char* s);
} 