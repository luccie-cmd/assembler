#include <slir/ir/function.h>

namespace assembler::ir::ir
{
Function::Function(std::string _name, size_t _arguments, FunctionBinding bind)
{
    this->name      = _name;
    this->arguments = _arguments;
    this->funcBind  = bind;
    if (this->arguments == (size_t)-1)
    {
        std::printf("ICE: Invalid function arguments (-1) provided to `%s`\n", name.c_str());
        std::exit(1);
    }
    if (this->arguments > 6)
    {
        std::printf("TODO: handle more than 6 args");
        std::exit(1);
    }
}
Function::~Function() {}
std::vector<const char*> nToArgMapping = {
    "rdi", "rsi", "rdx", "rcx", "r8", "r9",
};
static const char* nToArg(size_t i)
{
    if (i < nToArgMapping.size())
    {
        return nToArgMapping.at(i);
    }
    return "stack";
}
void Function::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("%lu function @%s(", (size_t)this->funcBind, this->name.c_str());
    for (size_t i = 0; i < this->arguments; ++i)
    {
        std::printf("i64 %%%s", nToArg(i));
        if (i != this->arguments - 1)
        {
            std::printf(", ");
        }
    }
    std::putchar(')');
    if (this->funcBind == FunctionBinding::Define)
    {
        std::printf(";\n");
        return;
    }
    std::printf("{\n");
    for (Block* block : this->blocks)
    {
        block->print(4 + spacing);
    }
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("}\n");
}
void Function::addBlock(Block* block)
{
    this->blocks.push_back(block);
}
}; // namespace assembler::ir::ir