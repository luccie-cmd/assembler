#if !defined(_ASSEMBLER_SLIR_IR_SECTION_H_)
#define _ASSEMBLER_SLIR_IR_SECTION_H_
#include "function.h"
#include "object.h"

#include <string>
#include <vector>

namespace assembler::ir::ir
{
class Section
{
  public:
    Section(std::string name);
    ~Section();
    void print();
    void addFunction(Function* func);
  private:
    std::string            name;
    std::vector<Function*> functions;
    std::vector<Object*>   objects;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_SECTION_H_
