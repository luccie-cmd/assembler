#include <clopts.h>
#include <cstdio>
#include <driver/context.h>
#include <driver/diag.h>
#include <filesystem>
#include <slir/ir/operand.h>
#include <string>

using namespace command_line_opts;
std::string                                                   inputFile;
std::string                                                   outputFile;
bool                                                          dumpAst;
bool                                                          dumpIr;
std::unordered_map<assembler::optimizations, bool>            enabledOpts;
std::vector<std::pair<std::string, assembler::optimizations>> stringOpts = {
    {"const-fold", assembler::optimizations::ConstantFolding},
};

assembler::optimizations stringToOpt(std::string opt)
{
    for (std::pair<std::string, assembler::optimizations> strOpt : stringOpts)
    {
        if (strOpt.first == opt)
        {
            return strOpt.second;
        }
    }
    return assembler::optimizations::Invalid;
}

void handleWarnings(std::string warning)
{
    std::printf("TODO warning: %s\n", warning.c_str());
}
void handleOptimizations(std::string opts)
{
    bool enable = true;
    if (opts.starts_with("no-"))
    {
        opts   = std::string((char*)((size_t)opts.c_str() + 3));
        enable = false;
    }
    assembler::optimizations opt = stringToOpt(opts);
    if (opt == assembler::optimizations::Invalid)
    {
        std::printf("error: Invalid optimization `%s`\n", opts.c_str());
        std::exit(1);
    }
    enabledOpts.insert_or_assign(opt, enable);
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
    if (std::filesystem::exists(path))
    {
        if (!inputFile.empty())
        {
            fprintf(stderr, "Cannot have multiple input files\n");
            return 1;
        }
        inputFile = path;
        return 0;
    }
    return 1;
}
clopts_opt_t clopts = {
    {{"-W", handleWarnings, false}, {"-f", handleOptimizations, false}, {"-o", setOutput, true}},
    unknownArg};

int main(int argc, char** argv)
{
    for (size_t i = 0; i < static_cast<size_t>(assembler::optimizations::End); ++i)
    {
        enabledOpts.insert_or_assign(static_cast<assembler::optimizations>(i), false);
    }
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
    assembler::Context*     ctx = new assembler::Context(contents, diagManager, outputFile, ob, of,
                                                         enabledOpts, dumpAst, dumpIr);
    ctx->start();
    return 0;
}