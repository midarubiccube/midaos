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

    ReadCPUID(&test, 0, 0);
    Log(kError, "eax: %04lx\n", test.eax);
    Log(kError, "ebx: %04lx\n", test.ebx);
    Log(kError, "ecx: %04lx\n", test.ecx);
    Log(kError, "edx: %04lx\n", test.edx);
  }
}
