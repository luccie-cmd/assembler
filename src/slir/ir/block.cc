#include <slir/ir/block.h>

namespace assembler::ir::ir
{
Block::Block(std::string _name)
{
    this->name = _name;
}
Block::~Block() {}
void Block::addInstruction(Instruction* inst)
{
    this->instructions.push_back(inst);
}
void Block::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("%s:\n", this->name.c_str());
    for (Instruction* inst : this->instructions)
    {
        inst->print(4 + spacing);
    }
}
std::vector<Instruction*> Block::getInstructions()
{
    return this->instructions;
}
}; // namespace assembler::ir::ir