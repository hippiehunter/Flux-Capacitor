#include <boost/shared_ptr.hpp>
#include <string>

#include "IDebugCommand.h"
#include "IVCPU.h"
#include "IMemory.h"
#include "ISymbolProvider.h"
#include "picojson.h"

using std::string;
using boost::shared_ptr;
using picojson::object;
using picojson::value;


namespace
{
class Reset : public IDebugCommand
{
private:
  shared_ptr<IVCPU> _vcpu;
  shared_ptr<IMemory> _memory;
  shared_ptr<ISymbolProvider> _symbolProvider;
public:
  Reset()
  {
    IDebugCommand::registerCommand(this);
  }
  
  virtual bool query_exec(std::string& command)
  {
    if(command == "reset")
      return true;
  }
  
  virtual void init(shared_ptr<IVCPU>& vcpu, shared_ptr<IMemory>& memory, shared_ptr<ISymbolProvider>& symbolProvider)
  {
    _vcpu = vcpu;
    _memory = memory;
    _symbolProvider = symbolProvider;
  }
  
  virtual value exec(object& commandPayload)
  {
    _memory->reset();
    
    return value(object { {"result", value("success")} });
  }
  
} instance;
}