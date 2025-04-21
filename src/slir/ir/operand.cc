#include <slir/ir/operand.h>

namespace assembler::ir::ir
{
Operand::Operand(OperandKind kind, Type* type, std::string reg)
{
    this->kind = kind;
    this->type = type;
    this->reg  = reg;
}
Operand::Operand(OperandKind kind, Type* type, uint64_t val)
{
    this->kind = kind;
    this->type = type;
    this->imm  = val;
}
OperandKind Operand::getOperandKind()
{
    return this->kind;
}
void Operand::print()
{
    if (this->type->kind == TypeKind::Integer)
    {
        std::printf("i%hhu ", this->type->bitSize);
    }
    else if (this->type->kind == TypeKind::Pointer)
    {
        std::printf("ptr ");
    }
    else if (this->type->kind == TypeKind::Label)
    {
        std::printf("label ");
    }
    else
    {
        std::printf("TODO: print TypeKind::%lu\n", (size_t)this->type->kind);
        std::exit(1);
    }
    if (this->kind == OperandKind::Register)
    {
        std::printf("%s", this->reg.c_str());
    }
    else if (this->kind == OperandKind::Immediate)
    {
        std::printf("%lu", this->imm);
    }
    else if (this->kind == OperandKind::SSA)
    {
        std::printf("%%%lu", this->imm);
    }
    else if (this->kind == OperandKind::Variable)
    {
        std::printf("@%s", this->reg.c_str());
    }
    else
    {
        std::printf("TODO: Print operandKind %lu\n", this->kind);
        std::exit(1);
    }
}
}; // namespace assembler::ir::ir