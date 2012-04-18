#include "VCPU.h"

#include <algorithm>
#include <stdexcept>

using std::fill;
using std::for_each;
using std::function;
using std::runtime_error;

void VCPU::reset()
{
  fill(_registers.begin(), _registers.end(), 0);
}

uint16_t VCPU::setRegister(IVCPU::VCPURegister reg, uint16_t value)
{
  _registers.at(reg) = value;
  return value;
}

uint16_t VCPU::getRegister(IVCPU::VCPURegister reg)
{
  return _registers.at(reg);
}

uint16_t VCPU::postIncrementRegister(IVCPU::VCPURegister reg)
{
  auto current = getRegister(reg);
  setRegister(reg, current + 1);
  return current;
}
uint16_t VCPU::preDecrementRegister(IVCPU::VCPURegister reg)
{
  return setRegister(reg, getRegister(reg) + 1);
}

void VCPU::run()
{
  while(_isRunning)
  {
    runStep();
    for_each(_watchers.begin(), _watchers.end(), 
      [&](std::function<void()>& func) { func(); });
  }
}

void VCPU::setBreak()
{
  _isRunning = false;
}

void VCPU::addStepWatcher(std::function<void()> watcher)
{
  _watchers.push_back(watcher);
}

void VCPU::runStep()
{
  uint16_t opCode = _memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC));
  
  uint32_t result;
  uint32_t arg2;
  function<uint16_t(uint16_t, bool)> dest;

  switch (opCode & OPCODE_MASK) 
  {
    case NON_BASIC:
      nbOpcode(opCode);
      break;
    case SET:
      argRef(opCode, ARG1)(argVal(opCode, ARG2), false);
      break;
    case ADD:
      dest = argRef(opCode, ARG1); 
      result = dest(0, true) + argVal(opCode, ARG2);
      dest(result, false);
      setRegister(IVCPU::VCPURegister::OF, result >> 16);
      break;
    case SUB:
      dest = argRef(opCode, ARG1);
      result = dest(0, true) - argVal(opCode, ARG2);
      dest(result, false);
      setRegister(IVCPU::VCPURegister::OF, result >>16);
      break;
    case MUL:
      dest = argRef(opCode, ARG1);
      result = dest(0, true) * argVal(opCode, ARG2);
      dest(result, false);
      setRegister(IVCPU::VCPURegister::OF, result >> 16);
      break;
    case DIV:
      dest = argRef(opCode, ARG1);
      arg2 = argVal(opCode, ARG2);
      setRegister(IVCPU::VCPURegister::OF, arg2 ? (dest(0, true)<<16) / arg2 : 0);
      dest((arg2 ? dest(0, true) / arg2 : 0), false);
      break;
    case MOD:
      dest = argRef(opCode, ARG1);
      arg2 = argVal(opCode, ARG2);
      dest((arg2 ? dest(0, true) % arg2 : 0), false);
      break;
    case SHL:
      dest = argRef(opCode, ARG1);
      result = dest(0, true) << argVal(opCode, ARG2);
      dest(result, false);
      setRegister(IVCPU::VCPURegister::OF, result >> 16);
      break;
    case SHR:
      dest = argRef(opCode, ARG1);
      arg2 = argVal(opCode, ARG2);
      setRegister(IVCPU::VCPURegister::OF, uint32_t(dest(0, true) << 16) >> arg2);
      dest(dest(0, true) >> arg2, false);
      break;
    case AND:
      dest = argRef(opCode, ARG1);
      dest(dest(0, true) & argVal(opCode, ARG2), false);
      break;
    case BOR:
      dest = argRef(opCode, ARG1);
      dest(dest(0, true) | argVal(opCode, ARG2), false);
      break;
    case XOR:
      dest = argRef(opCode, ARG1);
      dest(dest(0, true) ^ argVal(opCode, ARG2), false);
      break;
    case IFE:
      if (argVal(opCode, ARG1) != argVal(opCode, ARG2)) {
	opCode = _memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC));
	if (haveExtra(opCode, ARG1)) postIncrementRegister(IVCPU::VCPURegister::PC);
	if (haveExtra(opCode, ARG2)) postIncrementRegister(IVCPU::VCPURegister::PC);
      }
      break;
    case IFN:
      if (argVal(opCode, ARG1) == argVal(opCode, ARG2)) {
	opCode = _memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC));
	if (haveExtra(opCode, ARG1)) postIncrementRegister(IVCPU::VCPURegister::PC);
	if (haveExtra(opCode, ARG2)) postIncrementRegister(IVCPU::VCPURegister::PC);
      }
      break;
    case IFG:
      if (argVal(opCode, ARG1) <= argVal(opCode, ARG2)) {
	opCode = _memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC));
	if (haveExtra(opCode, ARG1)) postIncrementRegister(IVCPU::VCPURegister::PC);
	if (haveExtra(opCode, ARG2)) postIncrementRegister(IVCPU::VCPURegister::PC);
      }
      break;
    case IFB:
      if (! (argVal(opCode,ARG1) & argVal(opCode, ARG2))) {
	opCode = _memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC));
	if (haveExtra(opCode, ARG1)) postIncrementRegister(IVCPU::VCPURegister::PC);
	if (haveExtra(opCode, ARG2)) postIncrementRegister(IVCPU::VCPURegister::PC);
      }
      break;
    default:
      throw runtime_error("Invalid opcode.");
  }
}

void VCPU::nbOpcode(uint16_t opCode) {
  switch ((opCode & NB_OPCODE_MASK) >> NB_OPCODE_OFFSET) {
    case JSR:
      _memory->setAddress(preDecrementRegister(IVCPU::VCPURegister::SP), getRegister(IVCPU::VCPURegister::PC));
      setRegister(IVCPU::VCPURegister::PC, argVal(opCode, NBARG));
      break;
    default:
      throw runtime_error("Invalid opdcode.");
  };
}

function<uint16_t(uint16_t, bool)> VCPU::argRef(uint16_t opCode, ArgTypes type) {
  
  auto argOp = arg(opCode, type);
  
  IVCPU::VCPURegister targetRegister;
  switch(argOp)
  {
    case DRNW_A:
    case DR_A:
    case REG_A:
      targetRegister = IVCPU::VCPURegister::A;
      break;
    case DRNW_B:
    case DR_B:
    case REG_B:
      targetRegister = IVCPU::VCPURegister::B;
      break;
    case DRNW_C:
    case DR_C:
    case REG_C:
      targetRegister = IVCPU::VCPURegister::C;
      break;
    case DRNW_X:
    case DR_X:
    case REG_X:
      targetRegister = IVCPU::VCPURegister::X;
      break;
    case DRNW_Y:
    case DR_Y:
    case REG_Y:
      targetRegister = IVCPU::VCPURegister::Y;
      break;
    case DRNW_Z:
    case DR_Z:
    case REG_Z:
      targetRegister = IVCPU::VCPURegister::Z;
      break;
    case DRNW_I:
    case DR_I:
    case REG_I:
      targetRegister = IVCPU::VCPURegister::I;
      break;
    case DRNW_J:
    case DR_J:
    case REG_J:
      targetRegister = IVCPU::VCPURegister::J;
      break;
      
    case DRNW:
    case PC:
      targetRegister = IVCPU::VCPURegister::PC;
      break;
      
    case SP:
    case PEEK:
    case PUSH:
      targetRegister = IVCPU::VCPURegister::SP;
      break;
    default:
      throw runtime_error("Wrote to unwritable argument type.");
  }
  
  
  
  switch (argOp) {
    case REG_A:
    case REG_B:
    case REG_C:
    case REG_X:
    case REG_Y:
    case REG_Z:
    case REG_I:
    case REG_J:
    case SP:
    case PC: 
      return [&](uint16_t value, bool justGet) { return justGet ? getRegister(targetRegister) : setRegister(targetRegister, value); }; 
    case DR_A:
    case DR_B:
    case DR_C:
    case DR_X:
    case DR_Y:
    case DR_Z:
    case DR_I:
    case DR_J:
    case PEEK:
      return [&](uint16_t value, bool justGet) 
      { 
	return justGet ? _memory->getAddress(getRegister(targetRegister)) : 
	  _memory->setAddress(getRegister(targetRegister), value);
      };
    case DRNW_A:
    case DRNW_B:
    case DRNW_C:
    case DRNW_X:
    case DRNW_Y:
    case DRNW_Z:
    case DRNW_I:
    case DRNW_J:
      return [&](uint16_t value, bool justGet) 
      { 
	return justGet ?
	  _memory->getAddress(getRegister(targetRegister) + _memory->getAddress(getRegister(IVCPU::VCPURegister::PC))) :
	  _memory->setAddress(getRegister(targetRegister) + _memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC)), value);
      };
    case DRNW:
      return [&](uint16_t value, bool justGet)
      {
	return justGet ?
	  _memory->getAddress(_memory->getAddress(getRegister(IVCPU::VCPURegister::PC))) :
	  _memory->setAddress(_memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC)), value);
      };
    
    case PUSH:
      return [&](uint16_t value, bool justGet) 
      { 
	return justGet ? _memory->getAddress(getRegister(IVCPU::VCPURegister::SP) - 1) :
	  _memory->setAddress(setRegister(IVCPU::VCPURegister::SP, getRegister(IVCPU::VCPURegister::SP) - 1), value);
      };
    case POP:
      return [&](uint16_t value, bool justGet)
      {
	return justGet ?
	  _memory->getAddress(_memory->getAddress(getRegister(IVCPU::VCPURegister::SP))) :
	  _memory->setAddress(_memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::SP)), value);
      };
    default:
      throw runtime_error("Wrote to unwritable argument type.");
  }
}

uint16_t VCPU::argVal(uint16_t opCode, ArgTypes type) {
  switch (arg(opCode, type)) {
    case NW:
      return _memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC));
    case DRNW:
      return _memory->getAddress(_memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::PC)));
    case SP:
      return getRegister(IVCPU::VCPURegister::SP);
    case PC: 
      return getRegister(IVCPU::VCPURegister::PC);
    case PEEK:
      return _memory->getAddress(getRegister(IVCPU::VCPURegister::SP));
    case POP:
      return _memory->getAddress(postIncrementRegister(IVCPU::VCPURegister::SP));
    case PUSH:
      return _memory->getAddress(preDecrementRegister(IVCPU::VCPURegister::SP));
    case OF:
      return getRegister(IVCPU::VCPURegister::OF);
    default:
      if (arg(opCode, type) <= DRNW_J) {
        return argRef(opCode, type)(0, true);
      }
      if (arg(opCode, type) >= LITERAL_BEGIN) {
        return arg(opCode, type) - LITERAL_BEGIN;
      } 
      throw runtime_error("Read from write-only argument type.");
  }
}

bool VCPU::haveExtra(uint16_t opCode, ArgTypes type) {
  uint16_t a = arg(opCode, type);
  if ((a>=DRNW_A) && (a<=DRNW_J)) return true;
  if (a==NW) return true;
  if (a==DRNW) return true;
  return false;
}

uint16_t VCPU::arg(uint16_t opCode, ArgTypes type) {
  switch (type) {
    case ARG1:
      return (opCode & ARG1_MASK) >> ARG1_OFFSET;
    case ARG2:
      return (opCode & ARG2_MASK) >> ARG2_OFFSET;
    case NBARG:
      return (opCode & NB_ARG1_MASK) >> NB_ARG1_OFFSET;
    default:
      throw runtime_error("Logic Error");
  }
}