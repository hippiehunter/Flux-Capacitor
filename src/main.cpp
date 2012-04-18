#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


using boost::property_tree::write_json;
using boost::property_tree::json_parser::read_json;
using boost::property_tree::basic_ptree;
using std::cin;
using std::cout;


int main(int argc, char **argv) 
{
  std::stringstream inputBuffer;
  for(;;)
  {
    for(;;)
    {
      auto ch = cin.get();
      if(ch == '\0') //end of command
      {
	basic_ptree<std::string, std::string> pt;
	read_json(inputBuffer, pt);
	inputBuffer.clear();
	
	break;
      }
      inputBuffer << ch;
    }
    
    
    
  }
  return 0;
}
