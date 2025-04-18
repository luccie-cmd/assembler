#if !defined(_ASSEMBLER_OPTS_CONSTANT_FOLDING_H_)
#define _ASSEMBLER_OPTS_CONSTANT_FOLDING_H_
#include <driver/diag.h>
#include <string>
#include <syntax/ast/expr.h>
#include <syntax/ast/node.h>
#include <vector>
#include <optional>

// NOTE: Constant folding is the only one independant of anything as it's working with the AST and
// not the SLIR
namespace assembler::opts
{
class ConstantFoldingPass
{
  public:
    ConstantFoldingPass(bool fold);
    ~ConstantFoldingPass();
    std::vector<std::pair<DiagLevel, std::string>> run(std::vector<AstNode*> nodes);
    std::string                                    getName();

  private:
    std::optional<std::pair<DiagLevel, std::string>> foldExpr(ExpressionNode*& expr);
    std::optional<std::pair<DiagLevel, std::string>> foldNode(AstNode* node);
    bool                                             fold;
    std::string                                      name;
};
}; // namespace assembler::opts

#endif // _ASSEMBLER_OPTS_CONSTANT_FOLDING_H_
