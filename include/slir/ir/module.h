#if !defined(_ASSEMBLER_SLIR_IR_MODULE_H_)
#define _ASSEMBLER_SLIR_IR_MODULE_H_
#include "section.h"

#include <vector>

namespace assembler::ir::ir
{
class Module
{
  public:
    Module();
    ~Module();
    void print(bool dumpInternal);
    void addSection(Section* section);

  private:
    std::vector<Section*> sections;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_MODULE_H_
