#pragma once

#include <cstdint>

namespace apic {
  union IOAPIC_ID {
    uint32_t data;
    struct {
      uint32_t rsv0 : 15;
      uint8_t scratchpad : 1;
      uint8_t rsv1 : 8;
      uint8_t APIC_ID : 4;
      uint8_t rsv2 : 4;
    } __attribute__((packed)) bits;
  } __attribute__((packed));

  union IOAPIC_VER {
    uint32_t data;
    struct {
      uint32_t Version : 8;
      uint8_t rsv0 : 7;
      uint8_t PRQ : 1;
      uint8_t MRE : 8;
      uint8_t rsv1 : 8;
    } __attribute__((packed)) bits;
  } __attribute__((packed));

  union IOAPIC_REDIR_TBL {
    uint32_t data[2];
    struct {
      uint8_t Vector : 8;
      uint8_t Delivery_Mode : 3;
      uint8_t Destination_Mode : 1;
      uint8_t IIPP : 1;
      uint8_t Remote_IRR : 1;
      uint8_t Trigger_Mode : 1;
      uint8_t Mask : 1;
      uint32_t rsv0 : 31;
      uint8_t EDID : 8;
      uint8_t Destination : 1;
    } __attribute__((packed)) bits;
  } __attribute__((packed));
}