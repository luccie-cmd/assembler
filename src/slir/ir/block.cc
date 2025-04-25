#include <slir/ir/block.h>

namespace assembler::ir::ir
{
Block::Block(std::string _name)
{
    this->name = _name;
}
Block::~Block() {}
void Block::addInstruction(Instruction* inst)
{
    this->instructions.push_back(inst);
}
void Block::print(size_t spacing, bool dumpInternal)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("%s:", this->name.c_str());
    if (dumpInternal)
    {
        if (!this->predecessors.empty())
        {
            std::printf(" ; preds by: ");
            for (std::string pred : this->predecessors)
            {
                std::printf("%s ", pred.c_str());
            }
        }
        if (!this->successors.empty())
        {
            std::printf(" ; succs by: ");
            for (std::string pred : this->successors)
            {
                std::printf("%s ", pred.c_str());
            }
        }
    }
    std::printf("\n");
    if (dumpInternal)
    {
        for (std::pair<std::string, std::string> alias : this->_aliasses)
        {
            for (size_t i = 0; i < spacing + 4; ++i)
            {
                std::putchar(' ');
            }
            std::printf("Alias %s, %s\n", alias.first.c_str(), alias.second.c_str());
        }
    }
    for (Instruction* inst : this->instructions)
    {
        inst->print(4 + spacing);
    }
}
std::vector<Instruction*> Block::getInstructions()
{
    return this->instructions;
}
std::string Block::getName()
{
    return this->name;
}
void Block::addPredecessors(std::string pred)
{
    this->predecessors.push_back(pred);
}
void Block::addSuccessors(std::string sucs)
{
    this->successors.push_back(sucs);
}
void Block::replacePredecessor(std::string oldPred, std::string newPred)
{
    for (std::string& pred : this->predecessors)
    {
        if (pred == oldPred)
        {
            pred = newPred;
        }
    }
}
void Block::replaceSuccessor(std::string oldSucs, std::string newSucs)
{
    for (std::string& sucs : this->successors)
    {
        if (sucs == oldSucs)
        {
            sucs = newSucs;
        }
    }
}
std::vector<std::string> Block::getPredecessors()
{
    return this->predecessors;
}
std::vector<std::string> Block::getSuccessors()
{
    return this->successors;
}
std::vector<std::pair<std::string, std::string>> Block::getAliasses()
{
    return this->_aliasses;
}
void Block::addAliasses(std::vector<std::pair<std::string, std::string>> aliasses)
{
    this->_aliasses = aliasses;
}
void Block::addAlias(std::pair<std::string, std::string> alias)
{
    this->_aliasses.push_back(alias);
}
void Block::removeLastInst()
{
    this->instructions.pop_back();
}
}; // namespace assembler::ir::ir