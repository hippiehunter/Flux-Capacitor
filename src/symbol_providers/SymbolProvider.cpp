#include "ISymbolProvider.h"

#include <map>

using boost::shared_ptr;
using std::string;
using std::istream;
using std::map;
using std::function;

static map<string, function<ISymbolProvider*(istream&)>> registeredSymbolProviders;

void ISymbolProvider::registerSymbolProvider(function<ISymbolProvider*(istream&)> symbolProvider, const string& name)
{
  registeredSymbolProviders[name] = symbolProvider;
}

shared_ptr<ISymbolProvider> ISymbolProvider::loadSymbols(const string& name, istream& file)
{
  auto symbolProvider = registeredSymbolProviders.find(name);
  if(symbolProvider != registeredSymbolProviders.end())
    return shared_ptr<ISymbolProvider>(symbolProvider->second(file));
  
  else
    return shared_ptr<ISymbolProvider>();
}