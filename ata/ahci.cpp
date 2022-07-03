#include "ata/ahci.hpp"

#include "pci/pci.hpp"

#include "interrupt/interruptVectors.hpp"

#include "memory_manager.hpp"
#include "logger.hpp"
#include "timer.hpp"
#include "task.hpp"

namespace {
  using namespace ata;
  
  //GHCのログ
 void DumpAHCIRegisters(volatile GenericHostControl* ghc){
    Log(kError, "Command slot nun: %d\n", ghc->cap.bits.NCS + 1);
    Log(kError, "64-bit Addressing: %s\n", ghc->cap.bits.S64A ? "true" : "false");
    Log(kError, "Supports Staggered Spin-up: %s\n", ghc->cap.bits.SSS ? "true" : "false");
    Log(kError, "Supports AHCI mode only: %s\n", ghc->cap.bits.SPM ? "true" : "false");
    Log(kError, "Supports Port Multiplier: %s\n", ghc->cap.bits.SSS ? "true" : "false");
    Log(kError, "Supports Activity LED: %s\n", ghc->cap.bits.SAL ? "true" : "false");
    Log(kError, "Number of Command Slots: %d\n", ghc->cap.bits.NCS);
    Log(kError, "Supports Native Command Queuing %s\n", ghc->cap.bits.SNCQ ? "true" : "false");
  }
}

namespace ata {
  //コンストラクタ
  AHCIController::AHCIController(uint32_t abar)
    {
      ghc_ = reinterpret_cast<GenericHostControl*>(abar);
      port_register_base_ = reinterpret_cast<Port_Registers*>(abar + 0x100);
      for (uint8_t i = 0; i < 32; i++) {
        if ((ghc_->PI >> i) & 1){
          //ポートクラスをビープ領域に確保
          auto port = new PortController{i};
          port_.push_back(port);
        }
      }
      //コマンドリストを置く領域を確保する
      auto frame = memory_manager->Allocate(10).value;
      header_first_frame_id = frame.ID();
      command_heder_list_base_ = reinterpret_cast<Command_List*>(frame.Frame());
      //Recive fisを置く領域を確保する
      frame = memory_manager->Allocate(2).value;
      rsv_fis_first_frame_id = frame.ID();
      rsv_fis_base_ = reinterpret_cast<Receive_FIS*>(frame.Frame());
    }
  //デストラクタ
  AHCIController::~AHCIController() {
    memory_manager->Free(FrameID{header_first_frame_id}, 10);
    memory_manager->Free(FrameID{rsv_fis_first_frame_id}, 2);
  }

  //AHCIコントローラの初期化
  void AHCIController::Initialize() {
    //Statusの表示
    DumpAHCIRegisters(ghc_);
    //AHCIモード
    
    ghc_->ghc.bits.AE = 1;
    
    //初期化
    ghc_->ghc.bits.HR = 1;
     while (ghc_->ghc.bits.HR) {
        asm volatile("hlt");
    }
    //AHCIモード
    if (!ghc_->cap.bits.SPM){
      ghc_->ghc.bits.AE = 1;
    }

    Log(kDebug, "AHCI Initialize start version %x.%x\n", ghc_->VS.MJR, ghc_->VS.MNR);

    //64ビット対応か
    S64A = ghc->cap.bits.S64A;

    //Port個別に初期化
    for (auto port : port_) port->InitializeFirst();

    //Port個別に初期化
    for (auto port : port_) port->InitializeSecound();

    //クリアする
    ghc_->IS |= ghc_->IS;
    //ISをクリアされるまで待つ
    while (ghc_->IS);
 
    //割込みの許可
    ghc_->ghc.bits.IE = 1;

    //ATAコントローラを作成するよう指示する
    for (auto port : port_) port->MakeSATAController();
  
    Log(kError, "AHCI Initialize success\n");
  }

  void AHCIController::Interrupt() {
    for (auto port : port_){
      if ((ghc_->IS >> port->GetPortNumber()) & 1){
        //割込みが入っているポートに割込みの内容を調べる
        port->Interrupt();

        port->Clear_PxCI();
        if (port->GetType() == Porttype::ATA) {
          interrupt_que_.push_back({port->GetPortNumber(), 0});
        }
      }
    }
    //クリアする
    ghc_->IS |= ghc_->IS;
    //ISをクリアされるまで待つ
    while (ghc_->IS);
    
    __asm__("cli");
    task_ahci->SendMessage(Message{Message::kInterruptAHCI});
    __asm__("sti");
  }

  void AHCIController::InterruptMessage() {
    for (auto info : interrupt_que_) {
      port_[info.port]->InterruptMessage();
    }
    //割込みキューをクリアする
    interrupt_que_.clear();
  }
  
  //ウェイトを入れる
  void Wait(unsigned long time_out){
    __asm__("cli");
    //タイマーをセットする
    timer_manager->AddTimer(
      Timer{timer_manager->CurrentTick() + time_out, task_ahci->ID(), 3});
    __asm__("sti");
    
    //タスクのスリープ
    task_ahci->Sleep();
    while (true) {
      auto msg = task_ahci->ReceiveMessage();
      //kTimerTimeout以外は無視する
      if (msg->type == Message::kTimerTimeout) {
        break;
      }
      //再度スリープ
      task_ahci->Sleep();
    }
  }

  //ahci専用タスク
  void TaskAHCI(uint64_t task_id, int64_t data) {
    task_ahci = &task_manager->CurrentTask();
    //AHCIデバイスの検索
    pci::PCIDevice* ahci_dev = pci::FindDevice(0x01u, 0x06u);

    if (ahci_dev) {
      //見つかった場合
      Log(kInfo, "AHCI has been found: %d.%d.%d\n",
          ahci_dev->bus, ahci_dev->device, ahci_dev->function);
    } else {
      Log(kError, "AHCI has not been found\n");
    }

    //Local apic IDの取得
    const uint8_t bsp_local_apic_id =
      *reinterpret_cast<const uint32_t*>(0xfee00020) >> 24;
    //MSIの設定
    ahci_dev->ConfigureMSI(
        bsp_local_apic_id,
        interrupt::TriggerMode::kLevel, interrupt::DeliveryMode::kFixed,
        InterruptVector::kAHCI, 0);

    //コンフィグレーションレジスタのポインタ取得
    const uint32_t abar = 
        (ahci_dev->ReadBar(5).value) & ~static_cast<uint32_t>(0xfff);

    Log(kError, "ABAR = %p\n", abar);

    //ビープ領域に確保する
    controller = new AHCIController{abar};
    //初期化
    controller->Initialize();

    ghc = reinterpret_cast<GenericHostControl*>(abar);

    while (true) {
      __asm__("cli");
      auto msg = task_ahci->ReceiveMessage();
      if (!msg) {
        //タスクのスリープ
        task_ahci->Sleep();
        __asm__("sti");
        continue;
      }
      __asm__("sti");

      switch (msg->type) {
      case Message::kInterruptAHCI:
        Log(kError, "Interrupt Message came\n");
        controller->InterruptMessage();
        break;
      case Message::kAHCICommand:
      default:
        Log(kError, "Unknown Message type\n");
        break;
      }
    }
  }
}