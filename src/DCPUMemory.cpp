#include "DCPUMemory.h"

#include <algorithm>
#include <stdexcept>

using std::function;
using std::fill;
using std::for_each;
using std::runtime_error;
using std::vector;


void DCPUMemory::addWriteWatcher(std::function< void(uint16_t) > watcher)
{
  _writeWatchers.push_back(watcher);
}

uint16_t DCPUMemory::getAddress(uint16_t pos)
{
  return _ram.at(pos);
}

void DCPUMemory::reset()
{
  fill(_ram.begin(), _ram.end(), 0);
}

uint16_t DCPUMemory::setAddress(uint16_t pos, uint16_t value)
{
  _ram.at(pos) = value;
  
  for_each(_writeWatchers.begin(), _writeWatchers.end(),
    [&](function<void(uint16_t)>& arg) { return arg(pos); });
  return value;
}

void DCPUMemory::loadBin(std::istream& file)
{
  reset(); 

  size_t i = 0;
  auto dat = file.get();
  while (file.good()) {
    if (i >= NWORDS) {
      throw runtime_error("File exceeds DCPU RAM");
    }

    _ram.at(i) = (dat & 0xff) << 8;
    
    dat = file.get();
    if (file.fail()) {
      throw runtime_error("File has odd size");
    }
    _ram.at(i) |= dat & 0xff;

    dat = file.get();
    i++;
  }
  if (file.fail() && !file.eof()) {
    throw runtime_error("Unexpected end of file");
  }
}

vector<uint16_t> DCPUMemory::dumpBin()
{
  return vector<uint16_t>(_ram.begin(), _ram.end());
}




