#include "ISymbolProvider.h"

#include <map>

using boost::shared_ptr;
using std::string;
using std::istream;
using std::map;

static map<string, ISymbolProvider*> registeredSymbolProviders;

void ISymbolProvider::registerSymbolProvider(ISymbolProvider* symbolProvider, const string& name)
{
  registeredSymbolProviders[name] = symbolProvider;
}

ISymbolProvider* ISymbolProvider::loadSymbols(const string& name, istream& file)
{
  auto symbolProvider = registeredSymbolProviders.find(name);
  if(symbolProvider != registeredSymbolProviders.end())
  {
    symbolProvider->second->LoadSymbols(file);
    return symbolProvider->second;
  }
  else
    return nullptr;
}