#include <boost/shared_ptr.hpp>
#include <string>

#include "IDebugCommand.h"
#include "IVCPU.h"
#include "IMemory.h"
#include "ISymbolProvider.h"
#include "picojson.h"

using std::string;
using boost::shared_ptr;
using picojson::value;
using picojson::object;


namespace
{
class LoadSymbols : public IDebugCommand
{
private:
  shared_ptr<IVCPU> _vcpu;
  shared_ptr<IMemory> _memory;
  shared_ptr<ISymbolProvider> _symbolProvider;
public:
  LoadSymbols()
  {
    IDebugCommand::registerCommand(this);
  }
  
  virtual bool query_exec(string& command)
  {
    if(command == "symbols")
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
    auto binaryFileName = commandPayload["targetFile"].get<string>();
    ifstream file (binaryFileName, ios::in|ios::binary|ios::ate);
    if(file.is_open())
    {
      auto symbolProvider = ISymbolProvider::loadSymbols(commandPayload["providerIdentifier"].get<string>());
      IDebugCommand::initCommands(_vcpu, _memory, symbolProvider);
      return value(object {{"result", value("success")}});
    }
    else
    {
      return value(object
	{
	  {"result", value("failure")},
	  {"reason", value("file not found")}
	});
    }
  }
  
} instance;
}