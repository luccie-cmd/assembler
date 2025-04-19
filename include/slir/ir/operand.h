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
};
class Operand
{
  public:
  private:
    union
    {
        std::string reg;
    };
    OperandKind kind;
    Type*       type;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_OPERAND_H_
