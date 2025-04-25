#include <slir/ir/function.h>
#include <unordered_map>

namespace assembler::ir::ir
{
std::unordered_map<std::string, Block*> lazyCreationList;
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
static std::vector<const char*> nToArgMapping = {
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
void Function::print(size_t spacing, bool dumpInternal)
{
    if (!lazyCreationList.empty())
    {
        std::printf("ICE: Function `%s` lazy creation list not empty\n", this->name.c_str());
        std::exit(1);
    }
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    switch (this->funcBind)
    {
    case FunctionBinding::DeclareExternal:
    {
        std::printf("declare external");
    }
    break;
    case FunctionBinding::DeclareInternal:
    {
        std::printf("declare internal");
    }
    break;
    case FunctionBinding::Define:
    {
        std::printf("define");
    }
    break;
    default:
    {
        std::printf("Invalid function binding %lu\n", (size_t)this->funcBind);
        std::exit(1);
    }
    break;
    }
    std::printf(" function @%s(", this->name.c_str());
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
        block->print(4 + spacing, dumpInternal);
    }
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("}\n");
}
std::string Function::getName()
{
    return this->name;
}

void Function::addBlock(Block* block)
{
    Block* blk = block;
    if (lazyCreationList.contains(block->getName()))
    {
        blk = lazyCreationList.at(block->getName());
        for (Instruction* inst : block->getInstructions())
        {
            blk->addInstruction(inst);
        }
        for (std::string succ : block->getSuccessors())
        {
            blk->addSuccessors(succ);
        }
        for (std::string pred : block->getPredecessors())
        {
            blk->addPredecessors(pred);
        }
        lazyCreationList.erase(name);
    }
    this->blocks.push_back(blk);
}
Block* Function::getBlockByName(std::string blockName)
{
    for (Block* blk : this->blocks)
    {
        if (blk->getName() == blockName)
        {
            return blk;
        }
    }
    if (!lazyCreationList.contains(blockName))
    {
        lazyCreationList[blockName] = new Block(blockName); // dummy
    }
    return lazyCreationList[blockName];
}
std::vector<Block*> Function::getBlocks()
{
    return this->blocks;
}
}; // namespace assembler::ir::ir