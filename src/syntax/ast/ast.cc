#include <syntax/ast/ast.h>

namespace assembler
{
Ast::Ast(std::vector<AstNode*> _nodes)
{
    this->nodes = _nodes;
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