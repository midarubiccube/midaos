#include "ata/fis.hpp"

namespace ata {
  void Register_H2D::SetLBA(uint64_t lba) {
    lba0 = static_cast<uint8_t>(lba & 0xffu);
    lba1 = static_cast<uint8_t>((lba >> 8) & 0xffu);
    lba2 = static_cast<uint8_t>((lba >> 16) & 0xffu);
    lba3 = static_cast<uint8_t>((lba >> 24) & 0xffu);
    lba4 = static_cast<uint8_t>((lba >> 32) & 0xffu);
    lba5 = static_cast<uint8_t>((lba >> 40) & 0xffu);
  }

  void Register_H2D::SetCount(uint16_t count){
    count1 = static_cast<uint8_t>(count & 0xffu);
    counth = static_cast<uint8_t>((count >> 8) & 0xffu);
  }
}