#include <slir/ir/module.h>

namespace assembler::ir::ir
{
Module::Module() {}
Module::~Module() {}
void Module::print()
{
    for (Section* section : this->sections)
    {
        section->print();
    }
}
void Module::addSection(Section* section)
{
    this->sections.push_back(section);
}
} // namespace assembler::ir::ir