#pragma once

#include <array>
#include <deque>

#include "ata/fis.hpp"
#include "ata/registers.hpp"
#include "ata/SATAcontroller.hpp"

#include "task.hpp"
#include "logger.hpp"



namespace ata {
  class PortController {
  public:
    PortController(uint8_t port_num);
    
    //初期化メゾット1;
    void InitializeFirst();
    //初期化メゾット2;
    void InitializeSecound();
    //解除
    void UnInitialize();

    //ポートに対して割込みが入った際の処理
    void Interrupt();
    //割込みメッセージが入ったときの処理
    void InterruptMessage();
    //PxCIがクリアされるまで待つ
    void Clear_PxCI();
    //SATAのコントローラを作成する
    void MakeSATAController();
    //レジスタをダンプする
    void DumpPortRegister() const;

    //ATAタイプの取得
    inline Porttype GetType() const { return type_; }
    //ポートナンバーの取得

    inline uint8_t GetPortNumber() const { return port_number_; }
    inline bool ISController() const { return ata_controller_ != nullptr; }

    //割込みの内容をスタックする
    inline void StackInterruptType(ata::Interrupt_Type type) { interrupt_queue.push_back(type); Log(kError, "Interrupt type: %s\n", interrupt_type_[type]);};
  private:
    //コマンドリストの動作開始
    void PortEnable();
    //コマンドリストの動作停止
    void PortDisable();
    //エラーレジスタのクリア
    inline void ClearPxSERR();

    std::deque<ata::Interrupt_Type> interrupt_queue;

    //SATAのコントローラへのポインター
    SATAController* ata_controller_;
    //ポートレジスタの設定
    Port_Registers* register_;
    //ポートにつながれているデバイスのタイプ
    Porttype type_;
    //ポートの番号
    uint8_t port_number_;
  };

  inline uintptr_t memory_pool;
  inline uintptr_t alloc_ptr;
  extern Task* task_ahci;
  extern void Wait(unsigned long time_out);
}