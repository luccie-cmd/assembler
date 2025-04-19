#if !defined(_ASSEMBLER_SLIR_IR_FUNCTION_H_)
#define _ASSEMBLER_SLIR_IR_FUNCTION_H_
#include "block.h"

#include <string>
#include <vector>

namespace assembler::ir::ir
{
class Function
{
  public:
  private:
    std::string         name;
    size_t              arguments;
    std::vector<Block*> blocks;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_FUNCTION_H_
