#include "x64/cpuid.hpp"

#include "asmfunc.h"
#include "logger.hpp"
#include <cstdlib>

namespace {
  using namespace cpuid;
  struct CPUID_return{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
  }__attribute__((packed));
}

namespace cpuid {
  void Initialize() {
    const uint64_t rflags = ReadRFLAGS();
    Log(kError, "RFLAGS: %p\n", rflags);

    if (CheckSupportCPUID()) {
      Log(kError, "This CPU is support CPUID\n");
    } else {
      return;
    }

    CPUID_return test;
    

    ReadCPUID(&test, 0);
    uint32_t tmp[3];
    tmp[0] = test.ebx;
    tmp[1] = test.edx;
    tmp[2] = test.ecx;

    Log(kError, "CPU Vendor: %.12s\n", reinterpret_cast<const char*>(tmp));


    uint32_t tmp1[12];

    
    ReadCPUID(&test, 0x80000002);
    tmp1[0] = test.eax;
    tmp1[1] = test.ebx;
    tmp1[2] = test.ecx;
    tmp1[3] = test.edx;
    
    ReadCPUID(&test, 0x80000003);
    tmp1[4] = test.eax;
    tmp1[5] = test.ebx;
    tmp1[6] = test.ecx;
    tmp1[7] = test.edx;

    ReadCPUID(&test, 0x80000004);
    tmp1[8] = test.eax;
    tmp1[9] = test.ebx;
    tmp1[10] = test.ecx;
    tmp1[11] = test.edx;
    Log(kError, "CPU revision: %.48s\n", reinterpret_cast<const char*>(tmp1));
  }
}
