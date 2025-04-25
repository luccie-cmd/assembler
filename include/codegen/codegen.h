#if !defined(_ASSEMBLER_CODEGEN_CODEGEN_H_)
#define _ASSEMBLER_CODEGEN_CODEGEN_H_
#include <slir/ir/module.h>

namespace assembler::codegen
{
class CodeGen
{
  public:
    CodeGen(ir::ir::Module* _module);
    ~CodeGen();
    void genAsm();
  private:
    ir::ir::Module* _module;
};
}; // namespace assembler::codegen

#endif // _ASSEMBLER_CODEGEN_CODEGEN_H_
