#include <slir/ir/type.h>

namespace assembler::ir::ir
{
Type::Type(TypeKind _kind, uint8_t _bitSize)
{
    this->kind    = _kind;
    this->bitSize = _bitSize;
}
}; // namespace assembler::ir::ir