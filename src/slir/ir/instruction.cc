#include <slir/ir/instruction.h>

namespace assembler::ir::ir
{
static std::vector<std::pair<Opcode, std::string>> opcodeToStringPairs = {
    {Opcode::Add, "add"},     {Opcode::Imul, "imul"},      {Opcode::Load, "load"},
    {Opcode::Const, "const"}, {Opcode::Store, "store"},    {Opcode::Branch, "br"},
    {Opcode::Call, "call"},   {Opcode::Ret, "ret"},        {Opcode::Copy, "copy"},
    {Opcode::None, "none"},   {Opcode::IcmpEQ, "icmp eq"}, {Opcode::Select, "select"},
    {Opcode::Phi, "phi"}};

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
Instruction::Instruction(Opcode _opcode, std::vector<Operand*> _operands, std::string _result)
{
    this->opcode   = _opcode;
    this->operands = _operands;
    this->result   = _result;
}
Instruction::Instruction(Opcode _opcode, std::vector<Operand*> _operands)
{
    this->opcode   = _opcode;
    this->operands = _operands;
    this->result   = "";
}
Instruction::Instruction(Opcode _opcode, std::vector<std::pair<std::string, std::string>> _operands,
                         std::string _result, bool makeCShut)
{
    (void)makeCShut;
    this->opcode = _opcode;
    for (std::pair<std::string, std::string> operand : _operands)
    {
        this->phiOperands.push_back(
            {new Operand(OperandKind::Variable, new Type(TypeKind::Integer, 64), operand.first),
             operand.second});
    }
    this->result = _result;
}
std::vector<Operand*> Instruction::getOperands()
{
    return this->operands;
}
std::string Instruction::getResult()
{
    return this->result;
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
    if (!this->phiOperands.empty())
    {
        for (size_t i = 0; i < this->phiOperands.size(); ++i)
        {
            std::printf("[");
            this->phiOperands.at(i).first->print();
            std::printf(" %s", this->phiOperands.at(i).second.c_str());
            std::printf("]");
            if (i + 1 != this->operands.size())
            {
                std::printf(", ");
            }
        }
    }
    std::putchar(0x0a);
}
Opcode Instruction::getOpcode()
{
    return this->opcode;
}
} // namespace assembler::ir::ir
