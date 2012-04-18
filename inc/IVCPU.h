#ifndef IVCPU_H
#define IVCPU_H

#include <stdint.h>
#include <functional>

class IVCPU
{
public:
  enum VCPURegister
  {
    A,
    B,
    C,
    X,
    Y,
    Z,
    I,
    J,
    PC,
    SP,
    OF,
    REGISTER_COUNT
  };
  
  virtual void reset() = 0;
  virtual uint16_t setRegister(VCPURegister reg, uint16_t value) = 0;
  virtual uint16_t getRegister(VCPURegister reg) = 0;
  virtual void run() = 0;
  virtual void setBreak() = 0;
  virtual void addStepWatcher(std::function<void()> watcher) = 0;
};

#endif