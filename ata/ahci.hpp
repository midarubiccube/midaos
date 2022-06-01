#pragma once

#include <cstdint>
#include <vector>
#include <deque>

#include "ata/registers.hpp"
#include "ata/fis.hpp"
#include "ata/port.hpp"

#include "pci/pci.hpp"

#include "error.hpp"
#include "message.hpp"

namespace ata {
  struct Interrupt_Info{
    uint8_t port;
    uint8_t slot;
  };


  class AHCIController {
  public:
    //コンストラクタ
    AHCIController(uint32_t abar);
    //デストラクタ
    ~AHCIController();
    //初期化メンバ
    void Initialize();
    //割込みが入ったときの処理
    void Interrupt();
    void InterruptMessage();
    
    inline void SetIntteruptFlag(bool flag) { ghc_->ghc.bits.IE = flag ; }
  private:
    //コマンドリストを確保したページ番号
    size_t header_first_frame_id;
    size_t rsv_fis_first_frame_id;

    std::deque<Interrupt_Info> interrupt_que_;
  };

  //AHCIを制御するタスクの関数
  void TaskAHCI(uint64_t task_id, int64_t data);  
  //実装されているポートクラスへのポインター
  inline std::vector<PortController*> port_;
  //SATAがつながっているポートコントローラクラスのポインターのリスト
  inline std::vector<SATAController*> serial_ata_connect_ports;
  //コントローラクラスのポインター
  inline AHCIController* controller;
  //タスククラスへのポインター
  inline Task* task_ahci;
  inline GenericHostControl* ghc;
  //AHCIが64ビットに対応済みか
  inline bool S64A;
}