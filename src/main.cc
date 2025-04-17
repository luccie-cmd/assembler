#include <clopts.h>
#include <cstdio>
// #include <hash_map>
using namespace command_line_opts;
std::string inputFile;
std::string outputFile;
bool        dumpAst;
bool        dumpIr;

void handleWarnings(std::string warning)
{
    std::printf("TODO warning: %s\n", warning.c_str());
}
void handleOptimizations(std::string opts)
{
    std::printf("TODO optimizations: %s\n", opts.c_str());
}
void setOutput(std::string path)
{
    if (!outputFile.empty())
    {
        fprintf(stderr, "Cannot have multiple output files\n");
        exit(1);
    }
    outputFile = path;
}
int unknownArg(std::string path)
{
    if (path.starts_with("-dump-"))
    {
        std::string tree = path.substr(6);
        if (tree == "ast")
        {
            dumpAst = true;
        }
        if (tree == "ir")
        {
            dumpIr = true;
        }
        return 0;
    }
    if (!inputFile.empty())
    {
        fprintf(stderr, "Cannot have multiple input files\n");
        return 1;
    }
    inputFile = path;
    return 0;
}
clopts_opt_t clopts = {{{"-W", handleWarnings}, {"-f", handleOptimizations}, {"-o", setOutput}},
                       unknownArg};

#include <driver/context.h>
#include <driver/diag.h>
#include <string>

int main(int argc, char** argv)
{
    std::string             contents;
    assembler::outputBits   ob = assembler::outputBits::Q64;
    assembler::outputFormat of;
    clopts.parse(argc, argv);
    contents                            = clopts.handleFile(inputFile);
    ob                                  = assembler::outputBits::Q64;
    of                                  = (ob == assembler::outputBits::D32
                                               ? assembler::outputFormat::ELF32
                                               : (ob == assembler::outputBits::Q64 ? assembler::outputFormat::ELF64
                                                                                   : assembler::outputFormat::INVALID));
    assembler::DiagManager* diagManager = new assembler::DiagManager(inputFile, true, true);
    assembler::Context*     ctx = new assembler::Context(contents, diagManager, outputFile, ob, of);
    ctx->start();
    return 0;
}