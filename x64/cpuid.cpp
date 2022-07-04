#include "x64/cpuid.hpp"

#include <cstring>
#include <cstdlib>

#include "asmfunc.h"
#include "logger.hpp"

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
      Log(kError, "This CPU is not support CPUID\n");
      exit(1);
    }

    CPUID_return test;
    

    ReadCPUID(&test, 0);
    uint32_t tmp[3];
    tmp[0] = test.ebx;
    tmp[1] = test.edx;
    tmp[2] = test.ecx;

    cpu_vendor = reinterpret_cast<const char*>(tmp);

    Log(kError, "CPU Vendor: %.12s\n", cpu_vendor.c_str());


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

    ReadCPUID(&test, 0x1);

    Log(kError, "edx: %08lx\n", test.edx);
    edx_1.data = test.edx;

    cpu_revision = reinterpret_cast<const char*>(tmp1);
    Log(kError, "CPU revision: %.48s\n", cpu_revision.c_str());
  }
}
