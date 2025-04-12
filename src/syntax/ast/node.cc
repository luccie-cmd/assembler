#include <syntax/ast/node.h>

namespace assembler
{
AstNode::AstNode(AstNodeType type)
{
    this->astNodeType = type;
}
AstNode::~AstNode() {}
AstNodeType AstNode::getAstNodeType()
{
    return this->astNodeType;
}
}; // namespace assembler