#if !defined(_ASSEMBLER_SYNTAX_AST_NODE_H_)
#define _ASSEMBLER_SYNTAX_AST_NODE_H_
#include <cstddef>
#define SPACING_SIZE 2

namespace assembler
{
enum struct AstNodeType
{
    Declaration,
    Instruction,
    Expression,
    Direct,
};
class AstNode
{
  public:
    AstNode(AstNodeType _type);
    virtual ~AstNode()                     = 0;
    virtual void print(size_t spaceOffset) = 0;
    AstNodeType  getAstNodeType();

  protected:
    AstNodeType astNodeType;
};
}; // namespace assembler

#endif // _ASSEMBLER_SYNTAX_AST_NODE_H_
