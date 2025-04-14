#if !defined(_ASSEMBLER_SEMA_SYMBOL_H_)
#define _ASSEMBLER_SEMA_SYMBOL_H_
#include <string>
#include <vector>

namespace assembler
{
enum struct SymbolKind
{
    Unset,
    Object,
    Function,
    Section,
    Unknown,
};
enum struct SymbolBinding
{
    Global,
    Local,
    Extern,
};
class Symbol
{
  public:
    Symbol(SymbolBinding symbolBind, std::string name, bool isDefinedByLabel);
    ~Symbol();
    void          setSymbolKind(SymbolKind type);
    void          setIsDefinedByLabel(bool newState);
    SymbolKind    getSymbolKind();
    SymbolBinding getSymbolBind();
    std::string   getName();
    bool          getIsDefinedByLabel();

  private:
    SymbolKind    symbolKind;
    SymbolBinding symbolBind;
    std::string   name;
    bool          isDefinedByLabel;
};
class SymbolTable
{
  public:
    SymbolTable();
    ~SymbolTable();
    void    pushSymbol(Symbol* symbol);
    Symbol* getSymbolByName(std::string name);
    std::vector<Symbol*> getSymbols();

  private:
    std::vector<Symbol*> _symbols;
};
}; // namespace assembler

#endif // _ASSEMBLER_SEMA_SYMBOL_H_
