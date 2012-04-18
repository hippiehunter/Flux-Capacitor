#ifndef DCPU_REPLAY_H
#define DCPU_REPLAY_H

class IVCPU;
class IMemory;

class DCPUReplay
{
public:
  DCPUReplay(IVCPU* cpu, IMemory* memory);
private:
  IVCPU* _cpu;
  IMemory* _memory;
};


#endif