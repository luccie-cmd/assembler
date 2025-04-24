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
    Copy,

    Store,
    Load,

    Xor,
    Add,
    Sub,
    Imul,

    Branch,
    Call,
    Ret,

    Select,
    Phi,

    IcmpEQ,
    IcmpLE,
};
class Instruction
{
  public:
    Instruction(Opcode opcode, std::vector<Operand*> operands);
    Instruction(Opcode opcode, std::vector<std::pair<std::string, std::string>> operands, std::string result, bool makeCShut);
    Instruction(Opcode opcode, std::vector<Operand*> operands, std::string result);
    void   print(size_t spacing);
    Opcode getOpcode();
    std::vector<Operand*> getOperands();
    std::string getResult();

  private:
    std::string           result;
    Opcode                opcode;
    std::vector<Operand*> operands;
    std::vector<std::pair<ir::Operand*, std::string>> phiOperands;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_INSTRUCTION_H_
