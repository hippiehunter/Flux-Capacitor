#include "IDebugCommand.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <algorithm>
#include <vector>
#include <list>

using std::vector;
using std::list;
using boost::shared_ptr;
using std::for_each;


static vector<IDebugCommand*> registeredCommands;

void IDebugCommand::registerCommand(IDebugCommand* command)
{
  registeredCommands.push_back(command);
}

IDebugCommand* IDebugCommand::findCommand(std::string& name)
{
  list<IDebugCommand*> options;
  for_each(registeredCommands.begin(), registeredCommands.end(),
    [&](IDebugCommand* ptr)
    {
      if(ptr->query_exec(name))
	options.push_front(ptr);
    });
  
  if(options.size() != 1)
  {
    //oh noes it didnt work or was ambigious
  }
  else
    return options.front();
}
