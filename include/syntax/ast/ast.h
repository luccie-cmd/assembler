#if !defined(_ASSEMBLER_SYNTAX_AST_AST_H_)
#define _ASSEMBLER_SYNTAX_AST_AST_H_
#include "node.h"

#include <vector>

namespace assembler
{
class Ast
{
  public:
    Ast(std::vector<AstNode*> nodes);
    ~Ast();
    void                  print(bool dumpInternal);
    std::vector<AstNode*>& getNodes();

  private:
    std::vector<AstNode*> nodes;
};
} // namespace assembler

#endif // _ASSEMBLER_SYNTAX_AST_AST_H_
