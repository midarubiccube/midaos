#include "ata/registers.hpp"

#include "ata/ahci.hpp"

#include "logger.hpp"

namespace ata {
  void Command_List::Build_Header(
      Command_Table* table_ptr, uint8_t slot_num,
      uint16_t prdtl, uint16_t cfl, bool w)
  {
    memset(&header[slot_num], 0, 0x20);
    //テーブルのポインターを設定する
    header[slot_num].SetCTBA(table_ptr);
    //FISの長さを設定する
    header[slot_num].CFL = cfl;
    //Descriptor Tableを設定する
    header[slot_num].PRDTL = prdtl;
    //メモリに書き込むか
    header[slot_num].W = w;
    //コマンドの実行
    Log(kError, "Build Success\n");
  }

  void Command_Table::Build_Command_table(
      uint16_t count, void* dba) 
  {
    auto buf = reinterpret_cast<uint8_t*>(dba);

    uint16_t count1 = count;
    auto prdtl = (int)((count - 1) >> 4) + 1;
    int i;
    for (i = 0; i < prdtl - 1; i++) {
        PRDT[i].SetDBA(buf);
        PRDT[i].DBC = 8 * 1024 - 1;
        buf += 8 * 1024; // 8K bytes
        count1 -= 16; // 16 sectors
    }

    // Last entry
    PRDT[i].SetDBA(buf);
    PRDT[i].DBC = (count1 << 9) - 1;
    PRDT[i].I = 1;
  }

  void Command_Issus::SetCI(uint8_t slot) {
    CI |= 1 << slot;
  }


  void Port_Registers::SetPxCLB(void* pointer) {
    auto adress = reinterpret_cast<uint64_t>(pointer);
    if (S64A) {
      PxCLBU = static_cast<uint32_t>(adress>>32);
    }
    PxCLB = static_cast<uint32_t>(adress);
  }

  void Port_Registers::SetPxFB(void* pointer) {
    auto adress = reinterpret_cast<uint64_t>(pointer);
    if (S64A) {
      PxFBU = static_cast<uint32_t>(adress>>32);
    }
    PxFB = static_cast<uint32_t>(adress);
  }

  void Physical_Region_Descriptor_Table::SetDBA(void* pointer) {
    auto adress = reinterpret_cast<uint64_t>(pointer);
    if (S64A) {
      DBAU = static_cast<uint32_t>(adress>>32);
    }
    DBA = static_cast<uint32_t>(adress);
  }

  void Command_Header::SetCTBA(void* pointer) {
    auto adress = reinterpret_cast<uint64_t>(pointer);
    if (S64A) {
      CTBAU = static_cast<uint32_t>(adress>>32);
    }
    CTBA = static_cast<uint32_t>(adress);
  }

  void Serial_ATA_Active::SetTAG(uint8_t tag) {
    DS |= 1 << tag;
  }

  void IDENTIFY_DEVICE_DATA::replace(uint8_t* input) {
    for (int i = 0; i < 512; i+=2)
    { 
      //エンディアン変換を行う
      data8[i+1] = input[i];
      data8[i] = input[i+1];
    }
  }
}