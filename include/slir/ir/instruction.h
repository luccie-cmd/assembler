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
    
    Const,
    
    Store,
    Load,

    Xor,
    Add,
    Imul,
    
    Branch,
    Call,
    Ret,
};
class Instruction
{
  public:
    Instruction(Opcode opcode, std::vector<Operand*> operands);
    Instruction(Opcode opcode, std::vector<Operand*> operands, std::string result);
    void print(size_t spacing);
    Opcode getOpcode();
  private:
    std::string           result;
    Opcode                opcode;
    std::vector<Operand*> operands;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_INSTRUCTION_H_
