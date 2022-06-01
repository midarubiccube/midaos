#include "logger.hpp"

#include <cstddef>
#include <cstdio>

#include "console.hpp"
#include "serial/serial.hpp"

namespace {
  LogLevel log_level = kWarn;
}

extern Console* console;

void SetLogLevel(LogLevel level) {
  log_level = level;
}

int Log(LogLevel level, const char* format, ...) {
  

  va_list ap;
  int result;
  char s[1024];

  va_start(ap, format);
  result = vsprintf(s, format, ap);
  va_end(ap);

  SerialPort::PutAscii(s);

  if (level > log_level) {
    return 0;
  }

  console->PutString(s);
  
  return result;
}
