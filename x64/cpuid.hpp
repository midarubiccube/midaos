#pragma once

#include <cstdint>
#include <string>

namespace cpuid {
  void Initialize();

  union EDX_1 {
    uint32_t data;
    struct {
      uint8_t FPU:1;
      uint8_t VME:1;
      uint8_t DE:1;
      uint8_t PSE:1;
      uint8_t TSC:1;
      uint8_t MSR:1;
      uint8_t PAE:1;
      uint8_t MCE:1;
      uint8_t CX8:1;
      uint8_t APIC:1;
      uint8_t rsv0:1;
      uint8_t SEP:1;
      uint8_t MTRR:1;
      uint8_t PGE:1;
      uint8_t MCA:1;
      uint8_t CMOV:1;
      uint8_t PAT:1;
      uint16_t PSE_36:1;
      uint8_t PSN:1;
      uint8_t CLFSH:1;
    };
  };
  inline EDX_1 edx_1;

  inline std::string cpu_vendor;                                                                                                                                          
  inline std::string cpu_revision;

  inline bool CheckSupportAPIC() { return edx_1.APIC; }
  inline bool CheckSupportMSR() { return edx_1.MSR; }
}
 
