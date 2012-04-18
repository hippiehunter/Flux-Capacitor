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
  
  void insertStructureFields(uint16_t address, string typeName, basic_ptree<string, string>& response)
  {
    if(!type)
        type = "PTR";
      
    //command comes in the form of [address {type|WORD|DWORD|QWORD|PTR}]
    auto resolvedStructure = _symbolProvider->ResolveStructure(type);
    if(resolvedStructure != nullptr)
    {
      auto fields = resolvedStructure->AllFields();
      for(auto field : fields)
      {
        auto targetName = name + "." + field->Name();
        auto fieldAddress = address + field->OffsetFromParent();
        rslt.put<string>(targetName + ".readable", field->ReadableValue(fieldAddress));
      }
    }
  }
  
  virtual basic_ptree<string, string> exec(basic_ptree<string, string>& commandPayload)
  {
    basic_ptree<string, string> rslt;
    
    auto address = commandPayload.get_optional<string>("address");
    auto name = commandPayload.get_optional<string>("name");
    auto type = commandPayload.get_optional<string>("type");
    if(address)
    {
      insertStructureFields(address, type, rslt);
    }
    else if(name)
    {
      //name can be a register
      //command comes in the form of [name {type|WORD|DWORD|QWORD|PTR}]
      //this must be a global field name or a label (if we want to view the disassembly)
      auto foundFields = _symbolProvider->GlobalFields(name);
      if(foundFields.size() == 1)
      {
        auto globalField = foundFields.front();
        insertStructureFields(globalField->Address(), type ? type : globalField->Type()->Name(), rslt);
      }
      else if(foundFields.size() > 1)
      {
        rslt.put<string>("result", "failure");
        rslt.put<string>("reason", "ambigious symbol");
      }
      else 
      {
        auto foundLabel = _symbolProvider->ResolveLabel(name);
        if(foundLabel != nullptr)
        {
          //it looks like we want to disassemble the line that the label is from
          //TODO: this isnt a primary feature do it later
        }
      }
    }
    else
    {
      rslt.put<string>("result", "failure");
      rslt.put<string>("reason", "incorrect parameters");
    }
    

    return rslt;
  }
  
} instance;