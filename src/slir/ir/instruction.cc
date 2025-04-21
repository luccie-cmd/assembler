#include <slir/ir/instruction.h>

namespace assembler::ir::ir
{
static std::vector<std::pair<Opcode, std::string>> opcodeToStringPairs = {
    {Opcode::Add, "add"},     {Opcode::Imul, "imul"},   {Opcode::Load, "load"},
    {Opcode::Const, "const"}, {Opcode::Store, "store"}, {Opcode::Copy, "copy"},
    {Opcode::Branch, "br"},   {Opcode::Call, "call"},   {Opcode::Ret, "ret"}};

static std::string convertOpcodeToString(Opcode opcode)
{
    for (std::pair<Opcode, std::string> opcodeStringPair : opcodeToStringPairs)
    {
        if (opcodeStringPair.first == opcode)
        {
            return opcodeStringPair.second;
        }
    }
    std::printf("Opcode %lu not found\n", (size_t)opcode);
    std::exit(1);
}
Instruction::Instruction(Opcode opcode, std::vector<Operand*> operands, std::string result)
{
    this->opcode   = opcode;
    this->operands = operands;
    this->result   = result;
}
Instruction::Instruction(Opcode opcode, std::vector<Operand*> operands)
{
    this->opcode   = opcode;
    this->operands = operands;
    this->result   = "";
}
void Instruction::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    if (!this->result.empty())
    {
        std::printf("%s = ", this->result.c_str());
    }
    std::printf("%s ", convertOpcodeToString(this->opcode).c_str());
    for (size_t i = 0; i < this->operands.size(); ++i)
    {
        this->operands.at(i)->print();
        if (i + 1 != this->operands.size())
        {
            std::printf(", ");
        }
    }
    std::putchar(0x0a);
}
Opcode Instruction::getOpcode()
{
    return this->opcode;
}
} // namespace assembler::ir::ir
