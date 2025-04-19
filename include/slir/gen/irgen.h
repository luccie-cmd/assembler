#if !defined(_ASSEMBLER_SLIR_GEN_IRGEN_H_)
#define _ASSEMBLER_SLIR_GEN_IRGEN_H_
#include <syntax/ast/ast.h>
#include <sema/symbol.h>
#include <slir/ir/module.h>

namespace assembler::ir::gen
{
class IrGen
{
    public:
        IrGen(Ast* ast, SymbolTable* symTable);
        ~IrGen();
    private:
        Ast* _ast;
        SymbolTable* _symTable;
};
} // namespace assembler::ir::gen

#endif // _ASSEMBLER_SLIR_GEN_IRGEN_H_
