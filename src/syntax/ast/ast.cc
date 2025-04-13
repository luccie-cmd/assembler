#include <syntax/ast/ast.h>

namespace assembler
{
Ast::Ast(std::vector<AstNode*> nodes)
{
    this->nodes = nodes;
}
Ast::~Ast() {}
void Ast::print()
{
    for (AstNode* node : nodes)
    {
        node->print(0);
    }
}
std::vector<AstNode*>& Ast::getNodes()
{
    return this->nodes;
}
} // namespace assembler