#if !defined(_ASSEMBLER_SLIR_IR_BLOCK_H_)
#define _ASSEMBLER_SLIR_IR_BLOCK_H_
#include "instruction.h"

#include <string>
#include <vector>

namespace assembler::ir::ir
{
class Block
{
  public:
  private:
    std::string               name;
    std::vector<Instruction*> instructions;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_BLOCK_H_
