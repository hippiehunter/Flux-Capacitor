#ifndef VCPU_H
#define VCPU_H

#include "IVCPU.h"
#include "IMemory.h"

#include <array>
#include <vector>

class VCPU : public IVCPU
{
private:
  IMemory* _memory;
  std::array<uint16_t, IVCPU::VCPURegister::REGISTER_COUNT> _registers;
  std::vector<std::function<void()>> _watchers;
  bool _isRunning;
public:
  VCPU(IMemory* memory) : _memory(memory) { reset(); }
  
  
  virtual void reset();
  virtual uint16_t setRegister(IVCPU::VCPURegister reg, uint16_t value);
  virtual uint16_t getRegister(IVCPU::VCPURegister reg);
  virtual void run();
  virtual void step();
  virtual void setBreak();
  virtual void addStepWatcher(std::function<void()> watcher);
  
private:
  static const uint16_t OPCODE_MASK  = 0x000f;
  static const uint16_t ARG1_MASK    = 0x03f0;
  static const uint16_t ARG1_OFFSET  = 4;
  static const uint16_t ARG2_MASK    = 0xfc00;
  static const uint16_t ARG2_OFFSET  = 10;
  
  enum InstructionArguments 
  {
    /* Contents of register: */
    REG_A, REG_B, REG_C, REG_X, REG_Y, REG_Z, REG_I, REG_J,
    /* Dereference register: */
    DR_A, DR_B, DR_C, DR_X, DR_Y, DR_Z, DR_I, DR_J,
    /* Dereference (register + next word): */
    DRNW_A, DRNW_B, DRNW_C, DRNW_X, DRNW_Y, DRNW_Z, DRNW_I, DRNW_J,
    /* Others: */
    POP,
    PEEK,
    PUSH,
    SP,
    PC,
    OF, 
    DRNW, // dereference next word
    NW,   // next word
    LITERAL_BEGIN
  };

  static const size_t IMMEDIATE_MAX = 0x1f;

  enum BasicOpcodes {
    NON_BASIC,
    SET,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    SHL,
    SHR,
    AND,
    BOR,
    XOR,
    IFE,
    IFN,
    IFG,
    IFB
  };

  /* Structure of a nonbasic instruction */
  static const uint16_t NB_OPCODE_MASK   = ARG1_MASK;
  static const uint16_t NB_OPCODE_OFFSET = ARG1_OFFSET;
  static const uint16_t NB_ARG1_MASK     = ARG2_MASK;
  static const uint16_t NB_ARG1_OFFSET   = ARG2_OFFSET;

  enum NonBasicOpcodes {
    RESERVED,
    JSR,
    NB_OPCODE_END
  };
  
  enum ArgTypes {
    ARG1, ARG2, NBARG
  };
  
  uint16_t arg(uint16_t opCode, ArgTypes type);
  bool haveExtra(uint16_t opCode, ArgTypes type);
  uint16_t argVal(uint16_t opCode, ArgTypes type);
  std::function<uint16_t(uint16_t, bool)> argRef(uint16_t opCode, ArgTypes type);
  void nbOpcode(uint16_t opCode);
  uint16_t postIncrementRegister(IVCPU::VCPURegister reg);
  uint16_t preDecrementRegister(IVCPU::VCPURegister reg);
  
  void runStep();
  
};

#endif