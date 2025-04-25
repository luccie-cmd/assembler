#if !defined(_ASSEMBLER_SLIR_IR_FUNCTION_H_)
#define _ASSEMBLER_SLIR_IR_FUNCTION_H_
#include "block.h"

#include <string>
#include <vector>

namespace assembler::ir::ir
{
enum struct FunctionBinding
{
    DeclareInternal,
    DeclareExternal,
    Define,
};
class Function
{
  public:
    Function(std::string name, size_t arguments, FunctionBinding bind);
    ~Function();
    void        print(size_t spacing, bool dumpInternal);
    void        addBlock(Block* block);
    Block*      getBlockByName(std::string name);
    std::string getName();
    std::vector<Block*> getBlocks();

  private:
    std::string         name;
    size_t              arguments;
    FunctionBinding     funcBind;
    std::vector<Block*> blocks;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_FUNCTION_H_
