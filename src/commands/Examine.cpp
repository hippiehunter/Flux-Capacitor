#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <stdexcept>
#include <fstream>

#include "IDebugCommand.h"
#include "IVCPU.h"
#include "IMemory.h"
#include "ISymbolProvider.h"
#include "picojson.h"

using std::string;
using std::runtime_error;
using boost::shared_ptr;
using picojson::value;
using picojson::object;
using std::ifstream;
using std::ios;
using boost::lexical_cast;

namespace
{

class Examine : public IDebugCommand
{
private:
  shared_ptr<IVCPU> _vcpu;
  shared_ptr<IMemory> _memory;
  shared_ptr<ISymbolProvider> _symbolProvider;
public:
  Examine()
  {
    IDebugCommand::registerCommand(this);
  }
  
  virtual bool query_exec(std::string& command)
  {
    if(command == "examine")
      return true;
  }
  
  virtual void init(shared_ptr<IVCPU>& vcpu, shared_ptr<IMemory>& memory, shared_ptr<ISymbolProvider>& symbolProvider)
  {
    _vcpu = vcpu;
    _memory = memory;
    _symbolProvider = symbolProvider;
  }
  
  void insertStructureFields(uint16_t address, string baseName, string type, object& response)
  {      
    //command comes in the form of [address {type|WORD|DWORD|QWORD|PTR}]
    auto resolvedStructure = _symbolProvider->ResolveStructure(type);
    if(resolvedStructure != nullptr)
    {
      auto fields = resolvedStructure->AllFields();
      for(auto field : fields)
      {
        auto targetName = baseName + "." + field->Name();
        auto fieldAddress = address + field->OffsetFromParent();
        response.insert({targetName + ".readable", value(field->ReadableValue(fieldAddress))});
      }
    }
  }
  
  virtual value exec(object& commandPayload)
  {
    object rslt;
    
    auto addressItr = commandPayload.find("address");
    auto nameItr = commandPayload.find("name");
    auto typeItr = commandPayload.find("type");
    if(addressItr == commandPayload.end())
    {
      insertStructureFields(lexical_cast<uint16_t>(addressItr->second.get<string>()), 
			    "unnamed", 
			    typeItr == commandPayload.end() ? 
			      "PTR" :
			      typeItr->second.get<string>(), 
			    rslt);
    }
    else if(nameItr == commandPayload.end())
    {
      //name can be a register
      //command comes in the form of [name {type|WORD|DWORD|QWORD|PTR}]
      //this must be a global field name or a label (if we want to view the disassembly)
      auto foundFields = _symbolProvider->GlobalFields(nameItr->second.get<string>());
      if(foundFields.size() == 1)
      {
        auto globalField = foundFields.front();
        insertStructureFields(globalField->Address(), 
			      nameItr->second.get<string>(), 
			      typeItr == commandPayload.end() ? 
				globalField->Type()->Name() :
				typeItr->second.get<string>(), 
			      rslt);
      }
      else if(foundFields.size() > 1)
      {
        rslt.insert({"result", value("failure")});
        rslt.insert({"reason", value("ambigious symbol")});
      }
      else 
      {
        auto foundLabel = _symbolProvider->ResolveLabel(nameItr->second.get<string>());
        if(foundLabel != nullptr)
        {
          //it looks like we want to disassemble the line that the label is from
          //TODO: this isnt a primary feature do it later
        }
      }
    }
    else
    {
      rslt.insert({"result", value("failure")});
      rslt.insert({"reason", value("incorrect parameters")});
    }
    

    return value(rslt);
  }
  
} instance;
}