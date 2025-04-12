#include <cstdio>
#ifdef COMPILE
#include <clopts.hh>
using namespace command_line_options;
using options = clopts<
    flag<"--64", "Enable 64 bit compilation">, flag<"--32", "Enable 32 bit compilation">,
    option<"-o", "File to put the outputs into">,
    option<"-f", "Output format", values<"bin", "elf32", "elf64">>,
    positional<"file", "The file whose cont/ents should be parsed", file<>, /*required=*/true>,
    help<>>;
#endif
#include <driver/context.h>
#include <driver/diag.h>
#include <string>

int main(int argc, char** argv)
{
    std::string             contents, file, outfile;
    assembler::outputBits   ob;
    assembler::outputFormat of;
#ifdef COMPILE
    auto opts        = options::parse(argc, argv);
    contents         = opts.get<"file">()->contents;
    file             = opts.get<"file">()->path;
    outfile          = opts.get_or<"-o">("a.out");
    ob               = (opts.get<"--32">()
                            ? assembler::outputBits::D32
                            : (opts.get<"--64">() ? assembler::outputBits::Q64
                                                  : (sizeof(void*) == 8 ? assembler::outputBits::Q64
                                                                        : assembler::outputBits::D32)));
    of               = (ob == assembler::outputBits::D32
                            ? assembler::outputFormat::ELF32
                            : (ob == assembler::outputBits::Q64 ? assembler::outputFormat::ELF64
                                                                : assembler::outputFormat::INVALID));
    std::string* opt = opts.get<"-f">();
    if (opt != nullptr)
    {
        std::printf("TODO: overriding default format specifier\n");
    }
#endif
    assembler::DiagManager* diagManager = new assembler::DiagManager(file, true, true);
    assembler::Context*     ctx = new assembler::Context(contents, diagManager, outfile, ob, of);
    ctx->start();
    return 0;
}