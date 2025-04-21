#include <slir/ir/type.h>

namespace assembler::ir::ir
{
Type::Type(TypeKind kind, uint8_t bitSize)
{
    this->kind    = kind;
    this->bitSize = bitSize;
}
}; // namespace assembler::ir::ir