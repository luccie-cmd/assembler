#if !defined(_ASSEMBLER_SLIR_IR_INSTRUCTION_H_)
#define _ASSEMBLER_SLIR_IR_INSTRUCTION_H_
#include "operand.h"

#include <string>
#include <vector>

namespace assembler::ir::ir
{
enum struct Opcode
{
    None,
};
class Instruction
{
  public:
  private:
    std::string           result;
    Opcode                opcode;
    std::vector<Operand*> operands;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_INSTRUCTION_H_
