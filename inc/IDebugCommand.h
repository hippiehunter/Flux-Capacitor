#ifndef IDEBUG_COMMAND_H
#define IDEBUG_COMMAND_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "picojson.h"

class IVCPU;
class IMemory;
class ISymbolProvider;

class IDebugCommand
{
public:
  virtual bool query_exec(std::string& command) = 0;
  virtual void init(boost::shared_ptr<IVCPU>& vcpu, boost::shared_ptr<IMemory>& memory, boost::shared_ptr<ISymbolProvider>& symbolProvider) = 0;
  virtual picojson::value exec(picojson::object& commandPayload) = 0;
  static IDebugCommand* findCommand(std::string& name);
  static void initCommands(boost::shared_ptr<IVCPU>& vcpu, boost::shared_ptr<IMemory>& memory, boost::shared_ptr<ISymbolProvider>& symbols);
protected:
  static void registerCommand(IDebugCommand* command);
};


#endif