#include <codegen/codegen.h>

namespace assembler::codegen
{
CodeGen::CodeGen(ir::ir::Module* __module)
{
    this->_module = __module;
}
void CodeGen::genAsm() {}
}; // namespace assembler::codegen