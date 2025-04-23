#include <slir/ir/section.h>

namespace assembler::ir::ir
{
Section::Section(std::string _name)
{
    this->name = _name;
}
Section::~Section() {}
void Section::print()
{
    std::printf("%s {\n", this->name.c_str());
    for (Function* func : this->functions)
    {
        func->print(4);
    }
    std::puts("}");
}
void Section::addFunction(Function* func)
{
    this->functions.push_back(func);
}
}; // namespace assembler::ir::ir