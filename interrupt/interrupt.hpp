#pragma once

namespace Int {
  class IDTManager {
  public:
    IDTManager();
  private:
    
  };
  void InitializeInterrupt();
  void NotifyEndOfInterrupt();
}