#if !defined(_ASSEMBLER_SLIR_GEN_IRGEN_H_)
#define _ASSEMBLER_SLIR_GEN_IRGEN_H_
#include <driver/diag.h>
#include <ordered_map.h>
#include <sema/symbol.h>
#include <slir/ir/module.h>
#include <syntax/ast/ast.h>
#include <syntax/ast/expr.h>

namespace assembler::ir::gen
{
class IrGen
{
  public:
    IrGen(DiagManager* diagMngr, Ast* ast, SymbolTable* symTable);
    ~IrGen();
    ir::Module*  genModule();
    ir::Operand* genOperand(ExpressionNode* node);
    std::vector<ir::Instruction*> genExpr(ExpressionNode* node);
    Symbol* getSymbolByName(std::string name);

  private:
    std::vector<ir::Instruction*> genInstructions(AstNode* node);
    ir::Block*                    genBlock(std::string name, std::vector<AstNode*> nodes);
    ir::Function*                 genFunction(std::string name, std::vector<AstNode*> nodes);
    ir::Section*                  genSection(std::string name, std::vector<AstNode*> nodes);
    OrderedMap<std::string, std::vector<AstNode*>> sections;
    DiagManager*                                   _diagMngr;
    Ast*                                           _ast;
    SymbolTable*                                   _symTable;
};
} // namespace assembler::ir::gen

#endif // _ASSEMBLER_SLIR_GEN_IRGEN_H_
