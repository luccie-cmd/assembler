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
Symbol::Symbol(SymbolBinding _symbolBind, std::string _name, bool _isDefinedByLabel)
{
    this->symbolBind       = _symbolBind;
    this->symbolKind       = SymbolKind::Unset;
    this->isDefinedByLabel = _isDefinedByLabel;
    this->argumentCount    = static_cast<size_t>(-1);
    this->isChild          = false;
    this->name             = std::move(_name);
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
void Symbol::setIsDefinedByLabel(bool _isDefinedByLabel)
{
    this->isDefinedByLabel = _isDefinedByLabel;
}
void Symbol::setIsChild(bool _isChild)
{
    this->isChild = _isChild;
}
}; // namespace assembler