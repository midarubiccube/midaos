#include "serial/serial.hpp"

#include <cstdlib>

#include "logger.hpp"

#include "asmfunc.h"

namespace SerialPort {
  void Initialize() {
    IoOut8(COM1_PORT + 1, 0x00);
    IoOut8(COM1_PORT + 3, 0x80);
    IoOut8(COM1_PORT + 0, 0x03);
    IoOut8(COM1_PORT + 1, 0x00);
    IoOut8(COM1_PORT + 3, 0x02);
    IoOut8(COM1_PORT + 2, 0xC7);
    IoOut8(COM1_PORT + 4, 0x0B);
    IoOut8(COM1_PORT + 4, 0x1E);
    IoOut8(COM1_PORT + 0, 0xAE);
    IoOut8(COM1_PORT + 4, 0x0F);
    IoOut8(COM1_PORT + 0, 0xAE);

    if(IoIn8(COM1_PORT) != 0xAE) {
    }

    IoOut8(COM1_PORT, 0x0D);
    IoOut8(COM1_PORT, 0x0A);
  }

  void PutAscii(const char* s) {
    while (*s) {
      if (*s == '\n') {
        IoOut8(COM1_PORT, 0x0D);
        IoOut8(COM1_PORT, 0x0A);
      } else {
        IoOut8(COM1_PORT, static_cast<uint8_t>(*s));
      }
      ++s;
    }
  }
} 