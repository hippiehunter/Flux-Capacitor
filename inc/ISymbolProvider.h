#ifndef ISYMBOL_PROVIDER_H
#define ISYMBOL_PROVIDER_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <boost/weak_ptr.hpp>
#include <stdint.h>
#include <tuple>
#include <iostream>
#include <functional>

class ISymbolStructure;
class ISymbolField;
class ISymbolLabel;
class ISymbolSourceLine;
class ISymbolGlobalField;
class ISymbolStackFrame;

class ISymbolProvider
{
public:
  virtual boost::shared_ptr<ISymbolStructure> ResolveStructure(std::string& name) = 0;
  virtual std::vector<boost::shared_ptr<ISymbolGlobalField>> GlobalFields(std::string& name) = 0;
  virtual boost::shared_ptr<ISymbolLabel> ResolveLabel(std::string& name) = 0;
  
  virtual boost::shared_ptr<ISymbolField> ResolveGlobalField(uint16_t address) = 0;
  virtual boost::shared_ptr<ISymbolLabel> ResolveLabel(uint16_t address) = 0;
  
  virtual boost::shared_ptr<ISymbolSourceLine> ResolveSourceLine(uint16_t address) = 0;
  virtual boost::shared_ptr<ISymbolStackFrame> ResolveStackFrame(uint16_t address) = 0;
  virtual void AddSymbols(std::istream& file) = 0;
  static boost::shared_ptr<ISymbolProvider> loadSymbols(const std::string& name, std::istream& file);
protected:
  static void registerSymbolProvider(std::function<ISymbolProvider*(std::istream&)> symbolProvider, const std::string& name);
  
};

class ISymbolStackFrame
{
public:
  virtual std::string Name() = 0;
  virtual std::vector<boost::shared_ptr<ISymbolField>> Parameters() = 0;
  virtual std::vector<boost::shared_ptr<ISymbolField>> Variables() = 0;
  virtual boost::shared_ptr<ISymbolStackFrame> Parent() = 0;
  virtual boost::shared_ptr<ISymbolSourceLine> CurrentLine() = 0;
};

class ISymbolStructure
{
public:
  virtual std::string Name() = 0;
  virtual std::vector<boost::shared_ptr<ISymbolField>> AllFields() = 0;
  virtual boost::shared_ptr<ISymbolField> ResolveField(std::string& name) = 0;
};

class ISymbolField
{ 
public:
  virtual const std::string& Name() = 0;
  virtual std::vector<uint16_t> Bytes(uint16_t address) = 0;
  virtual std::string ReadableValue(uint16_t address) = 0;
  virtual boost::weak_ptr<ISymbolStructure> Owner() = 0;
  virtual boost::shared_ptr<ISymbolStructure> Type() = 0;
  virtual uint16_t OffsetFromParent() = 0;
};

class ISymbolGlobalField : public ISymbolField
{
public:
  virtual const std::string& Name() = 0;
  virtual std::vector<uint16_t> Bytes(uint16_t address) = 0;
  virtual std::string ReadableValue(uint16_t address) = 0;
  virtual boost::weak_ptr<ISymbolStructure> Owner() = 0;
  virtual boost::shared_ptr<ISymbolStructure> Type() = 0;
  virtual uint16_t OffsetFromParent() = 0;
  virtual uint16_t Address() = 0;
};

class ISymbolLabel
{
public:
  virtual uint16_t Address() = 0;
  virtual const std::string& Name() = 0;
};

class ISymbolSourceLine
{
public:
  virtual uint16_t Address() = 0;
  virtual const std::string& FileName() = 0;
  virtual std::tuple<uint32_t, uint32_t> FilePosition() = 0;
};

#endif