#if !defined(_ASSEMBLER_SYNTAX_AST_INST_H_)
#define _ASSEMBLER_SYNTAX_AST_INST_H_
#include "expr.h"
#include "node.h"

#include <cstdint>
#include <syntax/token.h>
#include <vector>

namespace assembler
{
class InstructionNode : public AstNode
{
  public:
    InstructionNode(Token* mnemonic, std::vector<AstNode*> args);
    ~InstructionNode();
    void                   print(size_t spaceOffset);
    std::vector<AstNode*>& getArgs();
    Token*                 getMnemonic();
    void                   setInstSize(uint8_t instSize);
    uint8_t                getInstSize();

  private:
    uint8_t               _instSize;
    Token*                _mnemonic;
    std::vector<AstNode*> _args;
};
} // namespace assembler

#endif // _ASSEMBLER_SYNTAX_AST_INST_H_
