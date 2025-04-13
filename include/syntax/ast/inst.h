#if !defined(_ASSEMBLER_SYNTAX_AST_INST_H_)
#define _ASSEMBLER_SYNTAX_AST_INST_H_
#include "node.h"

#include <syntax/token.h>
#include <vector>

namespace assembler
{
class InstructionNode : public AstNode
{
  public:
    InstructionNode(Token* mnemonic, std::vector<AstNode*> args);
    ~InstructionNode();
    void                  print(size_t spaceOffset);
    std::vector<AstNode*>& getArgs();

  private:
    Token*                _mnemonic;
    std::vector<AstNode*> _args;
};
} // namespace assembler

#endif // _ASSEMBLER_SYNTAX_AST_INST_H_
