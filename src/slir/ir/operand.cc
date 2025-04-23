#include <slir/ir/operand.h>

namespace assembler::ir::ir
{
Operand::Operand(OperandKind _kind, Type* _type, std::string _reg)
{
    this->kind = _kind;
    this->type = _type;
    this->reg  = std::move(_reg);
}
Operand::Operand(OperandKind _kind, Type* _type, uint64_t _imm)
{
    this->kind = _kind;
    this->type = _type;
    this->imm  = _imm;
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
        std::printf("%s", this->reg.c_str());
    }
    else
    {
        std::printf("TODO: Print operandKind %lu\n", (size_t)this->kind);
        std::exit(1);
    }
}
}; // namespace assembler::ir::ir