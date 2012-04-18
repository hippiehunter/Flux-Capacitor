#ifndef IDEBUG_COMMAND_H
#define IDEBUG_COMMAND_H

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <boost/shared_ptr.hpp>

class IVCPU;
class IMemory;
class ISymbolProvider;

class IDebugCommand
{
public:
  virtual bool query_exec(std::string& command) = 0;
  virtual void init(boost::shared_ptr<IVCPU>& vcpu, boost::shared_ptr<IMemory>& memory, boost::shared_ptr<ISymbolProvider>& symbolProvider) = 0;
  virtual boost::property_tree::basic_ptree<std::string, std::string> exec(boost::property_tree::basic_ptree<std::string, std::string>& commandPayload) = 0;
  static void registerCommand(IDebugCommand* command);
  static IDebugCommand* findCommand(std::string& name);
};


#endif