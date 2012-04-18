#ifndef MEMORY_LIST_MODEL_H
#define MEMORY_LIST_MODEL_H

#include <array>
#include <vector>
#include <functional>
#include "IMemory.h"

class DCPUMemory : public IMemory
{
public:
  DCPUMemory() {}
  
  virtual void reset();
  virtual uint16_t setAddress(uint16_t pos, uint16_t value);
  virtual uint16_t getAddress(uint16_t pos);
  virtual void addWriteWatcher(std::function<void(uint16_t)> watcher);
  virtual void loadBin(std::istream& file);
  virtual std::vector<uint16_t> dumpBin();
private:
  std::array<uint16_t, 65536> _ram; 
  std::vector<std::function<void(uint16_t)>> _writeWatchers;
  
  static const uint32_t NWORDS = 0x10000;
};


#endif