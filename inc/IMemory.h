#ifndef IMEMORY_H
#define IMEMORY_H

#include <stdint.h>
#include <functional>
#include <iostream>
#include <vector>

class IMemory
{
public:
  virtual void reset() = 0;
  virtual uint16_t setAddress(uint16_t pos, uint16_t value) = 0;
  virtual uint16_t getAddress(uint16_t pos) = 0;
  virtual void addWriteWatcher(std::function<void(uint16_t)> watcher) = 0;
  virtual void loadBin(std::istream& file) = 0;
  virtual std::vector<uint16_t> dumpBin() = 0;
};

#endif