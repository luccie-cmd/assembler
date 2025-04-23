#if !defined(_ASSEMBLER_SLIR_IR_OPERAND_H_)
#define _ASSEMBLER_SLIR_IR_OPERAND_H_
#include "type.h"

#include <cstdint>
#include <string>

namespace assembler::ir::ir
{
enum struct OperandKind
{
    Register,
    Immediate,
    SSA,
    Variable,
};
class Operand
{
  public:
    Operand(OperandKind kind, Type* type, std::string reg);
    Operand(OperandKind kind, Type* type, uint64_t imm);
    OperandKind getOperandKind();
    void print();
  private:
    std::string reg;
    uint64_t imm;
    OperandKind kind;
    Type*       type;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_OPERAND_H_
