#include <slir/ir/section.h>

namespace assembler::ir::ir
{
Section::Section(std::string name)
{
    this->name = name;
}
Section::~Section() {}
void Section::addFunction(Function* func)
{
    this->functions.push_back(func);
}
}; // namespace assembler::ir::ir