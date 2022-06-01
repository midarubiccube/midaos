#pragma once

#include <cstdint>
#include <array>

#include "ata/fis.hpp"

namespace ata {

  //AHCIデバイスの属性
  union CAP {
    uint32_t data;
    struct {
      uint32_t NP     : 5;
      uint32_t SXS    : 1;
      uint32_t EMS    : 1;
      uint32_t CCCS   : 1;
      uint32_t NCS    : 5;
      uint32_t PSC    : 1;
      uint32_t SSC    : 1;
      uint32_t PMD    : 1;
      uint32_t FBSS   : 1;
      uint32_t SPM    : 1;
      uint32_t SAM    : 1;
      uint32_t rsb0   : 1;
      uint32_t ISS    : 4;
      uint32_t SCLO   : 1;
      uint32_t SAL    : 1;
      uint32_t SALP   : 1;
      uint32_t SSS    : 1;
      uint32_t SMPS   : 1;
      uint32_t SSNFT  : 1;
      uint32_t SNCQ   : 1;
      uint32_t S64A   : 1; //64ビットサポートされているか
    }__attribute__((packed)) bits;
  }__attribute__((packed));
  //AHCIデバイスの制御レジスタ
  union GHC {
    uint32_t data;
    struct {
      uint32_t HR           : 1;// リセットレジスタ
      volatile uint32_t IE  : 1; //割込み許可レジスタ
      uint32_t MRSM         : 1; 
      uint32_t rsv0         : 28;
      uint32_t AE           : 1; //互換性用レジスタ
    } __attribute__((packed)) bits;
  }__attribute__((packed));

  //AHCIのバージョン
  struct Version {
    uint16_t MNR;
    uint16_t MJR;
  }__attribute__((packed));

  union CCC_CTL {
    uint32_t data;
    struct {
      uint32_t EN     : 1;
      uint32_t rsb0   : 2;
      uint32_t INT    : 5;
      uint32_t CC     : 8;
      uint32_t TV     : 16;
    }__attribute__((packed)) bits;
  }__attribute__((packed));

  union EM_LOC {
    uint32_t data;
    struct {
      uint32_t SZ     : 16;
      uint32_t OFST   : 16;
    }__attribute__((packed)) bits;
  }__attribute__((packed));

  union EM_CTL {
    uint32_t data;
    struct {
      uint32_t MR     : 1;
      uint32_t rsv0   : 7;
      uint32_t TM     : 1;
      uint32_t RST    : 1;
      uint32_t rsv1   : 6;
      uint32_t LED    : 1;
      uint32_t SAFTE  : 1;
      uint32_t SES2   : 1;
      uint32_t SGPIO  : 1;
      uint32_t rsv2   : 4;
      uint32_t SMB    : 1;
      uint32_t XMT    : 1;
      uint32_t ALHD   : 1;
      uint32_t PM     : 1;
      uint32_t rsv3   : 4;
    }__attribute__((packed)) bits;
  }__attribute__((packed));
  //AHCIデバイスの属性
  union CAP2 {
    uint32_t data;
    struct {
      uint32_t BOH    : 1;
      uint32_t NVMP   : 1;
      uint32_t APST   : 1;
      uint32_t SDS    : 1;
      uint32_t SAMD   : 1;
      uint32_t DESO   : 1;
      uint32_t rsv0   : 26;
    }__attribute__((packed)) bits;
  }__attribute__((packed));
  //BIOSから所有権移転するためのレジスタ
  union BOHC {
    uint32_t data;
    struct {
      uint32_t BB     : 1;
      uint32_t OOC    : 1;
      uint32_t SOOE   : 1;
      uint32_t OSS    : 1;
      uint32_t BOS    : 1;
      uint32_t rsv0   : 27;
    }__attribute__((packed)) bits;
  }__attribute__((packed));
  //AHCIを制御するレジスタ群
  struct GenericHostControl {
    //AHCIデバイスの属性1
    CAP cap;
    //AHCIデバイスのの制御レジスタ
    GHC ghc;
    //どのポートに割込みの指示があったかを示すレジスタ
    volatile uint32_t IS;
    uint32_t PI;
    //AHCIのバージョン
    Version VS;
    //
    CCC_CTL ccc_ctl;
    uint32_t ccc_ports;
    EM_LOC em_loc; 
    EM_CTL em_ctl;
    //AHCIデバイスの属性2
    CAP2 cap2;
    BOHC bohc;
  }__attribute__((packed));

  

  enum Interrupt_Type  {
    //D2hFISが送られてきた
    DHRS,
    //PIOSetupFISが送られてきた
    PSS,
    DSS,
    SDBS,
    UFS,
    DPS,
    PCS,
    DMPS,
    PRCS,
    IPMS,
    OFS,
    INFS,
    IFS,
    HBDS,
    HBFS,
    TFES,
    CPDS,
    //不明なタイプ
    Unknown_type,
  };

  union Command_Status{
    uint32_t data;
    struct {
      uint32_t ST     : 1;
      uint32_t SUD    : 1;
      uint32_t POD    : 1;
      uint32_t CLO    : 1;
      uint32_t FRE    : 1;
      uint32_t rsv0   : 3;
      uint32_t CCS    : 5;
      uint32_t MPSS   : 1;
      uint32_t FR     : 1;
      uint32_t CR     : 1;
      uint32_t CPS    : 1;
      uint32_t PMA    : 1;
      uint32_t HPCP   : 1;
      uint32_t MPSP   : 1;
      uint32_t CPD    : 1;
      uint32_t ESP    : 1;
      uint32_t FBSCP  : 1;
      uint32_t APSTE  : 1;
      uint32_t ATAPI  : 1;
      uint32_t DLAE   : 1;
      uint32_t ALPE   : 1;
      uint32_t ASP    : 1;
      uint32_t ICC    : 4;
    }__attribute__((packed)) bits;
  }__attribute__((packed));

  union Task_File_Data {
    uint32_t data;
    struct {
      uint32_t ERR1   : 1;
      uint32_t CS     : 2;
      uint32_t DRQ    : 1;
      uint32_t cs     : 3;
      uint32_t BSY    : 1;
      uint32_t ERR2   : 8;
      uint32_t rsv0   : 16;
    }__attribute__((packed)) bits;
  }__attribute__((packed));

  union Signature {
    uint32_t data;
    struct {
      uint8_t SCR;
      uint8_t LLR;
      uint8_t LMR;
      uint8_t LHR;
    }__attribute__((packed)) bits;
  }__attribute__((packed));

  union Serial_ATA_Status {
    uint32_t data;
    struct {
      uint32_t DET    : 4;
      uint32_t SPD    : 4;
      uint32_t IPM    : 4;
      uint32_t rsv0   : 20;
    }__attribute__((packed)) bits;
  }__attribute__((packed));

  union Serial_ATA_Error {
    volatile uint32_t data;
    struct {
      struct {
        uint32_t I : 1;
        uint32_t M : 1;
        uint32_t : 6;
        uint32_t T : 1;
        uint32_t C : 1;
        uint32_t P : 1;
        uint32_t E : 1;
        uint32_t : 4;
      }__attribute__((packed)) ERR;
      struct {
        uint32_t N : 1;
        uint32_t I : 1;
        uint32_t W : 1;
        uint32_t B : 1;
        uint32_t D : 1;
        uint32_t C : 1;
        uint32_t H : 1;
        uint32_t S : 1;
        uint32_t T : 1;
        uint32_t F : 1;
        uint32_t X : 1;
        uint32_t : 5;
      }__attribute__((packed)) DIAG;
    }__attribute__((packed)) bits;
  }__attribute__((packed));

  struct Serial_ATA_Active{
    uint32_t DS;

    void SetTAG(uint8_t tag);
  }__attribute__((packed));

  struct Command_Issus {
    volatile uint32_t CI;
    //slot番目のCIを1にする
    void SetCI(uint8_t slot);
  }__attribute__((packed));

  struct Port_Registers {
    uint32_t PxCLB;
    uint32_t PxCLBU;
    uint32_t PxFB;
    uint32_t PxFBU;
    volatile uint32_t PxIS;
    uint32_t PxIE;
    Command_Status PxCMD;
    uint32_t reserved1;
    Task_File_Data PxTFD;
    Signature PxSIG;
    Serial_ATA_Status PxSSTS;
    Serial_ATA_Status PxSCTL;
    Serial_ATA_Error PxSERR;
    Serial_ATA_Active PxSACT;
    Command_Issus PxCI;
    uint32_t PxSNTF;
    uint32_t PxFBS;
    uint32_t PxDEVSLP;
    uint32_t reserved[10];
	  uint32_t vendor_space[4];

    void SetPxCLB(void* pointer);

    void SetPxFB(void* pointer);
  }__attribute__((packed));
  
  struct Physical_Region_Descriptor_Table {
    uint32_t DBA;
    uint32_t DBAU;
    uint32_t rsv1;
    uint32_t DBC : 22; 
    uint32_t rsv2 : 9;
    uint32_t I : 1; 
    
    void SetDBA(void* pointer);
  }__attribute__((packed));

  struct Command_Header {
    uint8_t CFL : 5;
    uint8_t A : 1;
    uint8_t W : 1;
    uint8_t P : 1;
    uint8_t R : 1;
    uint8_t B : 1;
    uint8_t C : 1;
    uint8_t : 1;
    uint8_t PMP : 4;
    uint16_t PRDTL;
    uint32_t PRDBC;
    uint32_t CTBA;
    uint32_t CTBAU;
    uint32_t rsv[4];

    void SetCTBA(void* pointer);
  }__attribute__((packed));

  struct  Command_Table {
    uint64_t cfis[8];
    uint64_t acmd[2];
    uint64_t rsv[6];
    Physical_Region_Descriptor_Table PRDT[1];

    //コマンドテーブルを作成する
    void Build_Command_table(
      uint16_t count, void* dba
    );
  }__attribute__((packed));

  struct Command_List{
    Command_Header header[32];

    //コマンドヘッダーを作成する
    void Build_Header(
      Command_Table* table_ptr, uint8_t slot_num,
      uint16_t prdtl, uint16_t cfl, bool w);
  };

  enum Porttype {
    ATA,
    ATAPI,
    SEMB,
    PM,
    null,
    notfond,
  };
  
  static constexpr std::array port_type_{
    "ATA",
    "ATAPI",
    "SEMB",
    "PM",
    "null",
    "notfond",
  };

  static constexpr std::array interrupt_type_{
    "Device to Host Register FIS Interrupt Enable",
    "PIO Setup FIS Interrupt Enable",
    "DMA Setup FIS Interrupt Enable",
    "Set Device Bits FIS Interrupt Enable",
    "Unknown FIS Interrupt Enable",
    "Descriptor Processed Interrupt Enable",
    "Port Change Interrupt Enable",
    "Device Mechanical Presence Enable",
    "PhyRdy Change Interrupt Enable",
    "Incorrect Port Multiplier Enable"
    "Overflow Enable",
    "Interface Non-fatal Error Enable",
    "Interface Fatal Error Enable",
    "Host Bus Data Error Enable",
    "Host Bus Fatal Error Enable",
    "Task File Error Enable",
    "Cold Presence Detect Enable",
  };
  union IDENTIFY_DEVICE_DATA {
    //8ビットで読み込む
    uint8_t data8[512];
    //16ビットで読み込む
    uint16_t data16[256];

    struct{ 
      uint16_t rsv1[10];
      char Serial_Number[20];
      uint8_t rsv2[6];
      char Firmware_revision[8];
      char Model_number[40];
    } __attribute__((packed)) bits;
    void replace(uint8_t* input);
  }__attribute__((packed));

  //AHCIのコントローラへのポインター
  inline GenericHostControl* ghc_;
  //Portへのポインター
  inline Port_Registers* port_register_base_;

  inline Command_List* command_heder_list_base_;
  inline ata::Receive_FIS* rsv_fis_base_;
}