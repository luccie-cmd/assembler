#include <cstdio>
#include <syntax/ast/direct.h>

namespace assembler
{
DirectBytesNode::DirectBytesNode(uint8_t bytesPerArg, std::vector<AstNode*> args)
    : AstNode(AstNodeType::Direct)
{
    this->bytesPerArg = bytesPerArg;
    this->args        = args;
}
DirectBytesNode::~DirectBytesNode() {}
void DirectBytesNode::print(size_t spacing)
{
    (void)spacing;
    std::printf("TODO: Print direct bytes nodes\n");
}
}; // namespace assembler