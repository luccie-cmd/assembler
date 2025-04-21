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
    Block(std::string name);
    ~Block();
    void print(size_t spacing);
    void addInstruction(Instruction* inst);
    std::vector<Instruction*> getInstructions();
  private:
    std::string               name;
    std::vector<Instruction*> instructions;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_BLOCK_H_
