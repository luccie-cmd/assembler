#if !defined(_ASSEMBLER_DRIVER_CONTEXT_H_)
#define _ASSEMBLER_DRIVER_CONTEXT_H_
#include "diag.h"

#include <string>
#include <unordered_map>

namespace assembler
{
enum struct outputBits : int
{
    INVALID,
    W16,
    D32,
    Q64,
};
enum struct outputFormat : int
{
    INVALID,
    ELF32,
    ELF64,
    BINARY,
};
enum struct optimizations : int
{
    Invalid,
    ConstantFolding,
    End,
};
class Context
{
  public:
    Context(std::string contents, DiagManager* diagManager, std::string outfile, outputBits ob,
            outputFormat of, std::unordered_map<assembler::optimizations, bool> enabledOpts,
            bool dumpAst, bool dumpIR);
    ~Context();
    void start();

  private:
    std::string                                        _contents;
    DiagManager*                                       _diagManager;
    std::string                                        _outfile;
    outputBits                                         _ob;
    outputFormat                                       _of;
    bool                                               _dumpAst;
    bool                                               _dumpIR;
    std::unordered_map<assembler::optimizations, bool> _enabledOpts;
};
} // namespace assembler

#endif // _ASSEMBLER_DRIVER_CONTEXT_H_
