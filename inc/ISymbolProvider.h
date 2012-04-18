#ifndef ISYMBOL_PROVIDER_H
#define ISYMBOL_PROVIDER_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <boost/weak_ptr.hpp>
#include <stdint.h>
#include <tuple>

class ISymbolStructure;
class ISymbolField;
class ISymbolLabel;
class ISymbolSourceLine;
class ISymbolGlobalField;

class ISymbolProvider
{
public:
  virtual boost::shared_ptr<ISymbolStructure> ResolveStructure(std::string& name) = 0;
  virtual std::vector<boost::shared_ptr<ISymbolGlobalField>> GlobalFields(std::string& name) = 0;
  virtual boost::shared_ptr<ISymbolLabel> ResolveLabel(std::string& name) = 0;
  
  virtual boost::shared_ptr<ISymbolField> ResolveGlobalField(uint16_t address) = 0;
  virtual boost::shared_ptr<ISymbolLabel> ResolveLabel(uint16_t address) = 0;
  
  virtual boost::shared_ptr<ISymbolSourceLine> ResolveSourceLine(uint16_t address) = 0;
};

class ISymbolStructure
{
  virtual std::vector<boost::shared_ptr<ISymbolField>> AllFields() = 0;
  virtual boost::shared_ptr<ISymbolField> ResolveField(std::string& name) = 0;
};

class ISymbolField
{ 
  virtual const std::string& Name() = 0;
  virtual std::vector<uint16_t> Bytes(uint16_t address) = 0;
  virtual std::string ReadableValue(uint16_t address) = 0;
  virtual boost::weak_ptr<ISymbolStructure> Owner() = 0;
  virtual boost::shared_ptr<ISymbolStructure> Type() = 0;
};

class ISymbolGlobalField : public ISymbolField
{
  virtual const std::string& Name() = 0;
  virtual std::vector<uint16_t> Bytes(uint16_t address) = 0;
  virtual std::string ReadableValue(uint16_t address) = 0;
  virtual boost::weak_ptr<ISymbolStructure> Owner() = 0;
  virtual boost::shared_ptr<ISymbolStructure> Type() = 0;
  virtual uint16_t Address() = 0;
};

class ISymbolLabel
{
  virtual uint16_t Address() = 0;
  virtual const std::string& Name() = 0;
};

class ISymbolSourceLine
{
  virtual uint16_t Address() = 0;
  virtual const std::string& FileName() = 0;
  virtual std::tuple<uint32_t, uint32_t> FilePosition() = 0;
};

#endif