#pragma once

#include <cstdint>
#include <deque>
#include <string>

#include "ata/registers.hpp"
#include "ata/fis.hpp"

#include "task.hpp"

namespace ata {
  class PortController;

  //コマンドキューの情報
  struct Command_INFO {
    uint8_t slot;
    CommandType type;
  };

  class SATAController {
  public:
    //コンストラクタ
    SATAController(PortController* pc);
    //割込みメッセージが入った時の処理
    void InterruptMessage(Interrupt_Type type);
    
    //空きスロットの検索
    uint8_t FindFreeSlot();
    //H2DFISを構築
    void SendH2DFIS(
      CommandType type, uintptr_t lba,
      uintptr_t dba, uint16_t count, bool w);
    
    inline uint8_t GetPortnum() const { return Port_Number_; }
    inline IDENTIFY_DEVICE_DATA* GetInfo()  { return &dev_info; }
  private:
    //DHRS割込みが入ったときの処理
    void InterruptD2H();
    
    //エラーレジスタをクリアする
    inline void ClearPxSERR() { register_->PxSERR.data = register_->PxSERR.data; }
    
    //送ったコマンドのキュー
    std::deque<Command_INFO> command_que;

    //デバイスの情報
    IDENTIFY_DEVICE_DATA dev_info;
    
    //ポートをコントロールするレジスタ
    Port_Registers* register_;
    //コマンド発行するためのテーブル
    Command_List* command_list;
    //受信したFISを置くためのスペース
    Receive_FIS* Receive_FIS_;
    //ポートコントローラへのポインター
    PortController* pc_;
    //シリアルナンバー
    std::string Serial_Number;
    //Firmware_revision
    std::string Firmware_revision;
    //モデル番号
    std::string Model_number;
    //ポート番号
    uint8_t Port_Number_;
  };
}