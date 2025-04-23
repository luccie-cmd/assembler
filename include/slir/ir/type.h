#if !defined(_ASSEMBLER_SLIR_IR_TYPE_H_)
#define _ASSEMBLER_SLIR_IR_TYPE_H_
#include <cstdint>
#include <string>

namespace assembler::ir::ir
{
enum struct TypeKind
{
    Integer,
    Pointer,
    Label,
};
struct Type
{
    Type(TypeKind kind, uint8_t bitSize);
    ~Type();
    uint8_t  bitSize;
    TypeKind kind;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_TYPE_H_
