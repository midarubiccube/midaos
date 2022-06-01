#include "port.hpp"

#include <cstdint>
#include <string>

#include "ata/ahci.hpp"
#include "ata/SATAcontroller.hpp"

#include "logger.hpp"
#include "timer.hpp"
#include "memory_manager.hpp"

namespace {
  using namespace ata;

  #define	SATA_SIG_ATA	0x00000101	// SATA drive
  #define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
  #define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
  #define	SATA_SIG_PM	0x96690101	// Port multiplier
}

namespace ata {
  PortController::PortController(uint8_t port_num)
    : 
    ata_controller_{nullptr},
    register_{&port_register_base_[port_num]},
    type_{Porttype::null},
    port_number_{port_num}
    {}

  void PortController::InitializeFirst() {
    Log(kDebug, "InitializeFirst start in port number: %d\n", port_number_);
    PortEnable();
    //ゼロ埋める
    memset(&command_heder_list_base_[port_number_], 0, 0x400);
    memset(&rsv_fis_base_[port_number_], 0, 256);
    //レジスタにセットする
    register_->SetPxCLB(&command_heder_list_base_[port_number_]);
    register_->SetPxFB(&rsv_fis_base_[port_number_]);

    register_->PxSCTL.bits.IPM = 3;

    register_->PxIS = 0xffffffff;

    ClearPxSERR();

    //コールドプレゼンスに対応しているか
    if (register_->PxCMD.bits.CPD) {
      register_->PxCMD.bits.POD = 1;
    }
    //スッタガードスピンアップに対応しているか
    if (ghc_->cap.bits.SSS) {
      //スッタガードスピンアップ開始
      register_->PxCMD.bits.SUD = 1;
    }

    //デバイスの状態をActiveに設定
    register_->PxCMD.bits.ICC = 1;
    //receive fis の作動開始
    register_->PxCMD.bits.FRE = 1;
  }

  void PortController::InitializeSecound() {
    Log(kDebug, "InitializeSecound start in port number: %d\n", port_number_);
    
    PortDisable();

    ClearPxSERR();

    register_->PxIE = 0xfdc000ff;

    PortEnable();

    Wait(10);

    //再度COMRESETの実行が必要な場合
    if ((register_->PxTFD.data & 0x88) == 0){
      //COMRESETを送信する
      register_->PxSCTL.bits.DET = 1;
      //COMRESETを送信停止
      register_->PxSCTL.bits.DET = 0;
    } 

    PortDisable();

    Wait(10);
    
    if (register_->PxSSTS.bits.DET != 3){
      type_ = null;
    } else {
      switch (register_->PxSIG.data) {
      case SATA_SIG_ATA:
        type_ = ATA;
        break;
      case SATA_SIG_ATAPI:
        type_ = ATAPI;
        break;
      case SATA_SIG_SEMB:
        type_ = SEMB;
        break;
      case SATA_SIG_PM:
        type_ = PM;
        break;
      default:
        type_ = null;
        break;
      }
    }
    DumpPortRegister();
    
    //割込みを許可する前にエラーレジスタをクリアする
    ClearPxSERR();

    Interrupt();
    interrupt_queue.clear();
  }

  void PortController::UnInitialize() {
    
  }

  void PortController::MakeSATAController() {
    if (type_ == Porttype::ATA) {
      ata_controller_ = new SATAController{this};
      serial_ata_connect_ports.push_back(ata_controller_);
    }
  }
 
  void PortController::InterruptMessage() {
    if (!ata_controller_) return;

    for (auto type : interrupt_queue) {
      Log(kDebug, "interrupt came: %s\n", interrupt_type_[type]);
      ata_controller_->InterruptMessage(type);
    }

    interrupt_queue.clear();
  }

  void PortController::Clear_PxCI() {
    //クリアされるまで待つ
    if (register_->PxCI.CI) {
      Log(kError, "Clear PxCI\n");
      //while(register_->PxCI.CI);
      Log(kError, "Cleared PxCI\n");
    }
  }

  void PortController::PortDisable() {
    register_->PxCMD.bits.ST = 0;
    /*while (register_->PxCMD.bits.CR) {
      asm volatile ("hlt");
    }*/

    register_->PxCMD.bits.FRE = 1;
    register_->PxCMD.bits.ST = 1;
  }


  void PortController::DumpPortRegister() const {
    Log(kDebug, "Port number: 0x%d\n", port_number_);
	  Log(kDebug, "IE: 0x%08x ", register_->PxIE);
	  Log(kDebug, "IS: 0x%08x ", register_->PxIS);
	  Log(kDebug, "CMD: 0x%08x ", register_->PxCMD.data);
	  Log(kDebug, "SSTS: 0x%08x\n", register_->PxSSTS.data);
	  Log(kDebug, "SCTL: 0x%08x ", register_->PxSCTL.data);
	  Log(kDebug, "SERR: 0x%08x ", register_->PxSERR);
	  Log(kDebug, "SACT: 0x%08x ", register_->PxSACT);
	  Log(kDebug, "TFD: 0x%08x\n", register_->PxTFD.data);
  }

  void PortController::PortEnable() {
    if (register_->PxCMD.bits.ST != 1) {
      register_->PxCMD.bits.ST = 0;
      while(register_->PxCMD.bits.CR){
        asm volatile("hlt");
      }
    }
  }

  void PortController::Interrupt() {
    //クリアするデータを格納する変数
    volatile uint32_t clear_pxis = 0;
    for (uint8_t i = 0; i < 32; i++){
      if ((register_->PxIS >> i) & 1){
        //割込みの内容チェックする
        switch(i) {
        case 0:
          StackInterruptType(DHRS);
          clear_pxis |= 0x01;
          break;
        case 1:
          StackInterruptType(PSS);
          clear_pxis |= 1 << 1;
          break;
        case 2:
          StackInterruptType(DSS);
          clear_pxis |= 1 << 2;
          break;
        case 3:
          StackInterruptType(SDBS);
          clear_pxis |= 1 << 3;
          break;
        case 4:
          StackInterruptType(UFS);
          ClearPxSERR();
          break;
        case 5:
          StackInterruptType(DPS);
          clear_pxis |= 1 << 5;
          break;
        case 6:
          StackInterruptType(PCS);
          ClearPxSERR();
          break;
        case 7:
          StackInterruptType(DMPS);
          clear_pxis |= 1 << 7;
          break;
        case 22:
          StackInterruptType(PRCS);
          ClearPxSERR();
          break;
        case 23:
          StackInterruptType(IPMS);
          clear_pxis |= 1 << 23;
          break;
        case 24:
          StackInterruptType(OFS);
          clear_pxis |= 1 << 24;
          break;
        case 26:
          StackInterruptType(INFS);
          clear_pxis |= 1 << 26;
          break;
        case 27:
          StackInterruptType(IFS);
          clear_pxis |= 1 << 27;
          break;
        case 28:
          StackInterruptType(HBDS);
          clear_pxis |= 1 << 28;
          break;
        case 29:
          StackInterruptType(HBFS);
          clear_pxis |= 1 << 29;
          break;
        case 30:
          StackInterruptType(TFES);
          clear_pxis |= 1 << 30;
          break;
        case 31:
          StackInterruptType(CPDS);
          clear_pxis |= 1 << 31;
          break;
        default:
          break;
        } 
      }
    }
    
    register_->PxIS |= clear_pxis;
    while (register_->PxIS) asm volatile("hlt");
  }

  inline void PortController::ClearPxSERR() {
    //エラーレジスタをクリアする
    register_->PxSERR = register_->PxSERR;
  }
}