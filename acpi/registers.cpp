#include "acpi/registers.hpp"

#include <cstring>
#include <cstdlib>

#include "logger.hpp"

namespace {
using namespace acpi;

template <typename T>
uint8_t SumBytes(const T* data, size_t bytes) {
  return SumBytes(reinterpret_cast<const uint8_t*>(data), bytes);
}

template <>
uint8_t SumBytes<uint8_t>(const uint8_t* data, size_t bytes) {
  uint8_t sum = 0;
  for (size_t i = 0; i < bytes; ++i) {
    sum += data[i];
  }
  return sum;
}
inline std::vector<uint64_t> madt_list;
}


namespace acpi { 
  bool RSDP::IsValid() const {
    if (strncmp(this->signature, "RSD PTR ", 8) != 0) {
      Log(kDebug, "invalid signature: %.8s\n", this->signature);
      return false;
    }
    if (this->revision != 2) {
      Log(kDebug, "ACPI revision must be 2: %d\n", this->revision);
      return false;
    }
    if (auto sum = SumBytes(this, 20); sum != 0) {
      Log(kDebug, "sum of 20 bytes must be 0: %d\n", sum);
      return false;
    }
    if (auto sum = SumBytes(this, 36); sum != 0) {
      Log(kDebug, "sum of 36 bytes must be 0: %d\n", sum);
      return false;
    }
    return true;
  }

  bool DescriptionHeader::IsValid(const char* expected_signature) const {
    if (strncmp(this->signature, expected_signature, 4) != 0) {
      Log(kDebug, "invalid signature: %.4s\n", this->signature);
      return false;
    }
    if (auto sum = SumBytes(this, this->length); sum != 0) {
      Log(kDebug, "sum of %u bytes must be 0: %d\n", this->length,  sum);
      return false;
    }
    return true;
  }

  DescriptionHeader* XSDT::FindTable(const char* expected_signature) {
    for (int i = 0; i < Count(); ++i) {
      auto emtry = reinterpret_cast<DescriptionHeader*>(entry[i]);
      if (emtry->IsValid(expected_signature)) {
        return emtry;
      }
    }
    return nullptr;
  }

  void XSDT::CheckTable() const{
    for (int i = 0; i < Count(); ++i) {
      auto emtry = reinterpret_cast<DescriptionHeader*>(entry[i]);
      Log(kError, "signature: %.4s\n", emtry->signature);
    }
  }

  size_t XSDT::Count() const {
    return (this->header.length - sizeof(DescriptionHeader)) / sizeof(uint64_t);
  }

  void MADT_register::FindTable() {
    for 
    (
      auto i = reinterpret_cast<uint64_t>(&flags) + sizeof(flags);
      (i - reinterpret_cast<uint64_t>(&header) + 1) <= header.length;
      i += reinterpret_cast<Interrupt_StrctureHeader*>(i)->length
    )
    {
      madt_list.push_back(i);
      Log(kDebug, "%s\n", ics_names_[reinterpret_cast<Interrupt_StrctureHeader*>(i)->type]);
    }
  }

  size_t MADT_register::Count() const {
    return madt_list.size();
  }
  
  Interrupt_StrctureHeader* MADT_register::Check(size_t i) const {
    return reinterpret_cast<Interrupt_StrctureHeader*>(madt_list[i]);
  }
}