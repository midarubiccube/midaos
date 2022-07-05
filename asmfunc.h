#pragma once

#include <stdint.h>

extern "C" {
  void IoOut32(uint16_t addr, uint32_t data);
  uint32_t IoIn32(uint16_t addr);

  void IoOut8(uint16_t addr, uint8_t data);
  uint8_t IoIn8(uint16_t addr);

  void WriteRFLAGS(uint64_t rflags);
  uint64_t ReadRFLAGS();

  void ReadCPUID(void* return_ptr, uint32_t eax);

  uint64_t ReadMSR(uint32_t ecx);
  void WriteMSR();

  uint16_t GetCS(void);
  void LoadIDT(uint16_t limit, uint64_t offset);
  void LoadGDT(uint16_t limit, uint64_t offset);
  void SetCSSS(uint16_t cs, uint16_t ss);
  void SetDSAll(uint16_t value);
  void SetCR3(uint64_t value);
  uint64_t GetCR3();

  void SwitchContext(void* next_ctx, void* current_ctx);
  bool CheckSupportCPUID();
  void Disable8259PIC();
}
