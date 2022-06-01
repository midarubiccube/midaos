#pragma once

#include <array>
#include "ata/fis.hpp"

enum class LayerOperation {
  Move, MoveRelative, Draw, DrawArea
};

struct AHCI_Command {
  ata::CommandType command_type;
  uint64_t lba;
  uint64_t dba;
};

struct Message {
  enum Type {
    kInterruptXHCI,
    kTimerTimeout,
    kKeyPush,
    kLayer,
    kLayerFinish,
    kInterruptAHCI,
    kAHCICommand,
    kAHCI_Command_Finish
  } type;

  uint64_t src_task;

  union {
    AHCI_Command* command_info;

    struct {
      unsigned long timeout;
      int value;
    } timer;

    struct {
      uint8_t modifier;
      uint8_t keycode;
      char ascii;
    } keyboard;

    struct {
      LayerOperation op;
      unsigned int layer_id;
      int x, y;
      int w, h;
    } layer;
  } arg;
};


static constexpr std::array message_type_{
  "kInterruptXHCI",
  "kTimerTimeout",
  "kKeyPush",
  "kLayer",
  "kLayerFinish",
  "kAHCI_Interrput",
  "kAHCI_Command_Finish"
};
