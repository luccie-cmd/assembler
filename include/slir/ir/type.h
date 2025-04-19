#if !defined(_ASSEMBLER_SLIR_IR_TYPE_H_)
#define _ASSEMBLER_SLIR_IR_TYPE_H_
#include <cstdint>
#include <string>

namespace assembler::ir::ir
{
enum struct TypeKind
{
    Integer,
};
class Type
{
  public:
  private:
    union
    {
        uint8_t bitSize;
    };
    TypeKind kind;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_TYPE_H_
