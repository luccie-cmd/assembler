#include <syntax/ast/inst.h>

namespace assembler
{
InstructionNode::InstructionNode(Token* mnemonic, std::vector<AstNode*> args)
    : AstNode(AstNodeType::Instruction)
{
    this->_mnemonic = mnemonic;
    this->_args     = args;
}
InstructionNode::~InstructionNode() {}
void InstructionNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Instruction `%s`\n", this->_mnemonic->get_value().c_str());
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Arguments:\n");
    for (AstNode* node : this->_args)
    {
        node->print(spacing + (SPACING_SIZE * 2));
    }
}
std::vector<AstNode*> InstructionNode::getArgs()
{
    return this->_args;
}
}; // namespace assembler