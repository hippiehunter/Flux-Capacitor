#include <iostream>
#include <string>
#include "picojson.h"
#include "IDebugCommand.h"

using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using picojson::value;
using picojson::object;
using picojson::get_last_error;

int main(int argc, char **argv) 
{
  for(;;)
  {
    value v;
  
    // read json value from stream
    cin >> v;
    if (cin.fail()) 
    {
      cerr << get_last_error() << endl;
      continue;
    }
    
    if(v.is<object>())
    {
      auto command = v.get<object>();
      auto targetName = command.find("");
      if(targetName != command.end())
      {
	auto foundCommand = IDebugCommand::findCommand(targetName->second.get<string>());
	if(foundCommand != nullptr)
	{
	  cout << foundCommand->exec(command).serialize();
	  continue;
	}
      }
    }
  }
  return 0;
}
