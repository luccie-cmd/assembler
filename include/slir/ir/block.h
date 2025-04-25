#if !defined(_ASSEMBLER_SLIR_IR_BLOCK_H_)
#define _ASSEMBLER_SLIR_IR_BLOCK_H_
#include "instruction.h"

#include <string>
#include <vector>

namespace assembler::ir::ir
{
class Block
{
  public:
    Block(std::string name);
    ~Block();
    void                     print(size_t spacing, bool dumpInternal);
    void                     addInstruction(Instruction* inst);
    void                     addPredecessors(std::string predecessor);
    void                     addSuccessors(std::string successor);
    void                     replacePredecessor(std::string oldPred, std::string newPred);
    void                     replaceSuccessor(std::string oldSucs, std::string newSucs);
    void                     addAliasses(std::vector<std::pair<std::string, std::string>> aliasses);
    void                     addAlias(std::pair<std::string, std::string> alias);
    void                     removeLastInst();
    std::vector<std::string> getPredecessors();
    std::vector<std::string> getSuccessors();
    std::vector<Instruction*>                        getInstructions();
    std::string                                      getName();
    std::vector<std::pair<std::string, std::string>> getAliasses();

  private:
    std::vector<std::pair<std::string, std::string>> _aliasses;
    std::string                                      name;
    std::vector<Instruction*>                        instructions;
    std::vector<std::string>                         predecessors;
    std::vector<std::string>                         successors;
};
} // namespace assembler::ir::ir

#endif // _ASSEMBLER_SLIR_IR_BLOCK_H_
