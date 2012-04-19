#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

#include "IDebugCommand.h"
#include "IVCPU.h"
#include "IMemory.h"
#include "ISymbolProvider.h"

using std::string;
using boost::shared_ptr;
using boost::property_tree::basic_ptree;


namespace
{
class Step : public IDebugCommand
{
private:
  shared_ptr<IVCPU> _vcpu;
  shared_ptr<IMemory> _memory;
  shared_ptr<ISymbolProvider> _symbolProvider;
public:
  Step()
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
  
  virtual basic_ptree<string, string> exec(basic_ptree<string, string>& commandPayload)
  {
    _vcpu->step();
    
    basic_ptree<string, string> rslt;
    rslt.put<std::string>("result", "success");
    
    return rslt;
  }
  
} instance;
}