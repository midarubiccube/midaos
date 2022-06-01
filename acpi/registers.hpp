#pragma once

#include <cstdint>
#include <cstddef>

#include <vector>
#include <array>

namespace acpi {
  //Root System Description Pointer
  struct RSDP {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    char reserved[3];

    bool IsValid() const;
  } __attribute__((packed));
  
  //ACPI共通のヘッダー
  struct DescriptionHeader {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;

    bool IsValid(const char* expected_signature) const;
  } __attribute__((packed));

  struct XSDT {
  public:
    //基本的なヘッダー
    DescriptionHeader header;
    //各テーブルへのポインター
    uint64_t entry[];

    DescriptionHeader* FindTable(const char* expected_signature);
  private:
    size_t Count() const;
  } __attribute__((packed));
  
  struct FADT_register {
    DescriptionHeader header;

    uint32_t FIRMWARE_CTRL;
    uint32_t DSDT;
    char rsv0[2];
    uint16_t SCI_INT;
    uint32_t SMI_CMD;
    uint8_t ACPI_ENABLE;
    uint8_t ACPI_DISABLE;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_CNT;
    uint32_t PM1a_EVT_BLK;
    uint32_t PM1b_EVT_BLK;
    uint32_t PM1a_CNT_BLK;
    uint32_t PM1b_CNT_BLK;
    uint32_t PM2_CNT_BLK;
    uint32_t PM_TMR_BLK;
    uint32_t GPE0_BLK;
    uint32_t GPE1_BLK;
    uint8_t PM1_EVT_LEN;
    uint8_t PM1_CNT_LEN;
    uint8_t PM2_CNT_LEN;
    uint8_t PM_TMR_LEN;
    uint8_t GPE0_BLK_LEN;
    uint8_t GPE1_BLK_LEN;
    uint8_t GPE1_BASE;
    uint8_t CST_CNT;
    uint16_t P_LVL2_LAT;
    uint16_t P_LVL3_LAT;
    uint16_t FLUSH_SIZE;
    uint16_t FLUSH_STRIDE;
    uint8_t DUTY_OFFSET;
    uint8_t DUTY_WIDTH;
    uint8_t DAY_ALRM;
    uint8_t MON_ALRM;
    uint8_t CENTURY;
    uint16_t IAPC_BOOT_ARCH;
    char rsv1;
    uint32_t Flags;
    uint8_t RESET_REG[12];
    uint8_t RESET_VALUE;
    uint16_t ARM_BOOT_ARCH;
    uint8_t FADT_Minor_Version;
    uint64_t X_FIRMWARE_CTRL;
    uint64_t X_DSDT;
    uint8_t X_PM1a_EVT_BLK[12];
    uint8_t X_PM1b_EVT_BLK[12];
    uint8_t X_PM1a_CNT_BLK[12];
    uint8_t X_PM1b_CNT_BLK[12];
    uint8_t X_PM2_CNT_BLK[12];
    uint8_t X_PM_TMR_BLK[12];
    uint8_t X_GPE0_BLK[12];
    uint8_t X_GPE1_BLK[12];
    uint8_t SLEEP_CONTROL_REG[12];
    uint8_t SLEEP_STATUS_REG[12];
    uint64_t Hypervisor_Vendor_Identity;
  } __attribute__((packed));

  enum class ICSType;
  struct Interrupt_StrctureHeader;

  struct MADT_register {
    DescriptionHeader header;

    uint32_t local_int_ctl_addr;
    //PICコントローラもサポートする場合1
    uint32_t flags;
    
    void FindTable();

    size_t Count() const;
    Interrupt_StrctureHeader* Check(size_t i) const;
  } __attribute__((packed));

  //MADTのヘッダー
  struct Interrupt_StrctureHeader {
  
    uint8_t type;
    uint8_t length;
  } __attribute__((packed));

  struct IOAPIC_register {
    Interrupt_StrctureHeader header;

    uint8_t IOAPIC_ID;
    uint8_t rsv0;
    uint32_t IOAPIC_addr;
    uint32_t GSI;
  } __attribute__((packed));

  struct InterruptSourceOverride_register {
    Interrupt_StrctureHeader header;
    uint8_t bus;
    uint8_t source;
    uint32_t GSI;
    struct {
      uint8_t Polarity : 2;
      uint8_t Trigger_Mode : 2;
      uint16_t rsv0 : 12;
    } __attribute__((packed)) flag;
  } __attribute__((packed));
  
  //DSDTのレジスター
  struct DSDT_register {
    DescriptionHeader header;
    char Definition_Block;
  } __attribute__((packed));

  enum class ICSType {
    Processor_Local_APIC,
    IO_APIC,
    Interrupt_Source_Override,
    NMI,
    Local_APIC_NMI,
    Local_APIC_Address_Override,
    IO_SAPIC,
    Local_SAPIC,
    Platform_Interrupt_Sources,
    Processor_Local_x2APIC,
    Local_x2APIC_NMI,
    GIC_CPU_Interface,
    GIC_Distributor,
    GIC_MSI_Frame,
    GIC_Redistributor,
    GIC_Interrupt_Translation_Service
  };
  
  static constexpr std::array ics_names_ {
    "Processor_Local_APIC",
    "IO_APIC",
    "Interrupt_Source_Override",
    "NMI",
    "Local_APIC_NMI",
    "Local_APIC_Address_Override",
    "IO_SAPIC",
    "Local_SAPIC",
    "Platform_Interrupt_Sources",
    "Processor_Local_x2APIC",
    "Local_x2APIC_NMI",
    "GIC_CPU_Interface",
    "GIC_Distributor",
    "GIC_MSI_Frame",
    "GIC_Redistributor",
    "GIC_Interrupt_Translation_Service",
  };
}