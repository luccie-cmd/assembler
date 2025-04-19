#if !defined(_ASSEMBLER_SLIR_IR_OBJECT_H_)
#define _ASSEMBLER_SLIR_IR_OBJECT_H_
#include <cstdint>
#include <string>
#include <vector>

namespace assembler::ir::ir
{
class Object
{
  public:
  private:
    std::string          name;
    std::vector<uint8_t> bytes;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_OBJECT_H_
