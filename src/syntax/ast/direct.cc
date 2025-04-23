#include <cstdio>
#include <syntax/ast/direct.h>

namespace assembler
{
DirectBytesNode::DirectBytesNode(uint8_t _bytesPerArg, std::vector<AstNode*> _args)
    : AstNode(AstNodeType::Direct)
{
    this->bytesPerArg = _bytesPerArg;
    this->args        = _args;
}
DirectBytesNode::~DirectBytesNode() {}
void DirectBytesNode::print(size_t spacing)
{
    (void)spacing;
    std::printf("TODO: Print direct bytes nodes\n");
}
}; // namespace assembler