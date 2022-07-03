#pragma once

#include <cstdint>
#include <string>

namespace cpuid {
  void Initialize();
  inline std::string cpu_vendor;                                                                                                                                          
  inline std::string cpu_revision;
}
 
