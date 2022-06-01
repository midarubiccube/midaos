#pragma once

#include <cstdint>

namespace ata {
  struct Register_H2D {
    uint8_t fis_type; //FISのタイプ
    uint8_t pmport : 4; //複数のポートをサポートしているか
    uint8_t rsv1: 3; 
    uint8_t c : 1;

    uint8_t command; //コマンドのタイプ
    uint8_t featurel;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;

    uint8_t device;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;

    uint8_t featureh;
    uint8_t count1;
    uint8_t counth;
    uint8_t icc;
    uint8_t control;
    uint8_t rsv2[4];

    void SetLBA(uint64_t lba); //LBAを設定するメンバ関数
    void SetCount(uint16_t count);
  }__attribute__((packed));

  struct Register_D2H {
    uint8_t fis_type;
    
    uint8_t  pmport : 4;   
	  uint8_t  rsv0 : 2;
	  uint8_t  i : 1;
	  uint8_t  rsv1 : 1;
 
	  uint8_t  status;
	  uint8_t  error;
 
	  uint8_t  lba0;
	  uint8_t  lba1;
	  uint8_t  lba2;
	  uint8_t  device;
 
	  uint8_t  lba3;
	  uint8_t  lba4;
	  uint8_t  lba5;
	  uint8_t  rsv2; 
 
	  uint8_t  countl;
	  uint8_t  counth;

	  uint8_t  rsv3[2];

	  uint8_t  rsv4[4];
  }__attribute__((packed));

  struct DMA_Active_FIS {
    uint8_t fis_type;

    uint8_t pm_port:4;
    uint8_t rsv0:4;
    uint16_t rsv1;
  }__attribute__((packed));

  struct DMA_Setup_FIS
  {
    uint8_t fis_type; 
    uint8_t pmport:4;

    uint8_t R:1;
    uint8_t D:1;
    uint8_t I:1;
    uint8_t A:1;

    uint16_t rsv0;

    uint64_t DMAbufferID;
    uint32_t rsv1;
    uint32_t DMAbufOffset;
    uint32_t DMATransferCount;

    uint32_t rsv2;
  }__attribute__((packed));

  struct Data_FIS
  {
    uint8_t fis_type;
    uint8_t  pmport:4;	// Port multiplier
	  uint8_t  rsv0:4;		// Reserved
 
	  uint8_t  rsv1[2];	// Reserved
 
	  // DWORD 1 ~ N
	  uint32_t data[1];
  }__attribute__((packed));

  struct PIO_Setup_FIS {
    uint8_t fis_type; 
    uint8_t pmport:4;

    uint8_t R:1;
    uint8_t D:1;
    uint8_t I:1;
    uint8_t A:1;

    uint8_t status;
    uint8_t Error;

    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;

    uint8_t device;

    uint8_t lba3;
    uint8_t lba4;
    uint8_t lba5;

    uint8_t rsv0;

    uint8_t count0;
    uint8_t count1;
    
    uint8_t rsv1;
    uint8_t E_status;

    uint16_t transder_count; //送りたいデータのサイズ
    uint16_t rsv2;

  }__attribute__((packed));

  struct Set_Device_Bits {
    uint8_t fis_type; 
    uint8_t pmport:4;

    uint8_t N:1;
    uint8_t I:1;
    uint8_t R0:1;
    uint8_t R1:1;

    uint8_t status_lo:3;
    uint8_t R2:1;
    uint8_t status_hi:3;
    uint8_t R3:1;

    uint8_t Error;
    
    uint32_t Protocol_Specific;
  }__attribute__((packed));

  struct Receive_FIS {
    DMA_Setup_FIS DSFIS;

    uint8_t rsv0[4];

    PIO_Setup_FIS PSPIO;

    uint8_t rsv1[12];

    Register_D2H RFIS;

    uint8_t rsv2[4];

    Set_Device_Bits SDBFIS;

    uint64_t UFIS[8];
    uint64_t rsv3[12];
  }__attribute__((packed));

  enum CommandType {
    //デバイス情報の取得
    IDENTIFY_DEVICE = 0xecu,
    //セクターの読み出し
    READ_DMA_EXT = 0x25u,
  };

  enum FIS_Type {
    H2D_register_num = 0x27u,
    D2H_register_num = 0x34u,
    DMA_Activate_num = 0x39u,
    DMA_Setup_num = 0x41u,
    Data_FIS_num = 0x46u,
    BIST_Active_num = 0x58u,
    PIO_Setup_FIS_num = 0x5fu,
    Set_Device_Bits_FIS_num = 0xa1u,
  };
}