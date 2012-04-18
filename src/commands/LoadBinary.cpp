#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <stdexcept>
#include <fstream>

#include "IDebugCommand.h"
#include "IVCPU.h"
#include "IMemory.h"
#include "ISymbolProvider.h"

using std::string;
using std::runtime_error;
using boost::shared_ptr;
using boost::property_tree::basic_ptree;
using std::ifstream;
using std::ios;

class LoadBinary : public IDebugCommand
{
private:
  shared_ptr<IVCPU> _vcpu;
  shared_ptr<IMemory> _memory;
  shared_ptr<ISymbolProvider> _symbolProvider;
public:
  LoadBinary()
  {
    IDebugCommand::registerCommand(this);
  }
  
  virtual bool query_exec(std::string& command)
  {
    if(command == "load")
      return true;
  }
  
  virtual void init(shared_ptr<IVCPU>& vcpu, shared_ptr<IMemory>& memory, shared_ptr<ISymbolProvider>& symbolProvider)
  {
    _vcpu = vcpu;
    _memory = memory;
    _symbolProvider = symbolProvider;
  }
  
  virtual basic_ptree<string, string> exec(basic_ptree<string, string>& commandPayload)
  {
    basic_ptree<string, string> rslt;
    auto binaryFileName = commandPayload.get<std::string>("target");
    ifstream file (binaryFileName, ios::in|ios::binary|ios::ate);
    if(file.is_open())
    {
      _memory->loadBin(file);
      rslt.put<std::string>("result", "success");
    }
    else
    {
      rslt.put<std::string>("result", "failure");
      rslt.put<std::string>("reason", "file not found");
    }

    return rslt;
  }
  
} instance;