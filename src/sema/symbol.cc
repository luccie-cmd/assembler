#include <sema/symbol.h>

namespace assembler
{
SymbolTable::SymbolTable()
{
    this->_symbols.clear();
}
SymbolTable::~SymbolTable()
{
    for (Symbol* sym : this->_symbols)
    {
        delete sym;
    }
    this->_symbols.clear();
}
void SymbolTable::pushSymbol(Symbol* sym)
{
    this->_symbols.push_back(sym);
}
Symbol* SymbolTable::getSymbolByName(std::string name)
{
    for (Symbol* sym : this->_symbols)
    {
        if (sym->getName() == name)
        {
            return sym;
        }
    }
    return nullptr;
}
std::vector<Symbol*> SymbolTable::getSymbols()
{
    return this->_symbols;
}
Symbol::Symbol(SymbolBinding symbolBind, std::string name, bool isDefinedByLabel)
{
    this->symbolBind       = symbolBind;
    this->symbolKind       = SymbolKind::Unset;
    this->name             = name;
    this->isDefinedByLabel = isDefinedByLabel;
    this->argumentCount    = -1;
    this->isChild          = false;
}
Symbol::~Symbol() {}
std::string Symbol::getName()
{
    return this->name;
}
SymbolKind Symbol::getSymbolKind()
{
    return this->symbolKind;
}
SymbolBinding Symbol::getSymbolBind()
{
    return this->symbolBind;
}
bool Symbol::getIsDefinedByLabel()
{
    return this->isDefinedByLabel;
}
bool Symbol::getIsChild()
{
    return this->isChild;
}
size_t Symbol::getArgumentsCount()
{
    return this->argumentCount;
}
void Symbol::setArgumentsCount(size_t count)
{
    this->argumentCount = count;
}
void Symbol::setSymbolKind(SymbolKind kind)
{
    this->symbolKind = kind;
}
void Symbol::setIsDefinedByLabel(bool isDefinedByLabel)
{
    this->isDefinedByLabel = isDefinedByLabel;
}
void Symbol::setIsChild(bool isChild)
{
    this->isChild = isChild;
}
}; // namespace assembler