#if !defined(_ASSEMBLER_SYNTAX_AST_DIRECT_H_)
#define _ASSEMBLER_SYNTAX_AST_DIRECT_H_
#include "node.h"

#include <cstdint>
#include <vector>

namespace assembler
{
class DirectBytesNode : public AstNode
{
  public:
    DirectBytesNode(uint8_t bytesPerArg, std::vector<AstNode*> args);
    ~DirectBytesNode();
    void print(size_t spaceOffset);

  private:
    uint8_t               bytesPerArg;
    std::vector<AstNode*> args;
};
}; // namespace assembler

#endif // _ASSEMBLER_SYNTAX_AST_DIRECT_H_
