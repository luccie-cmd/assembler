#include <driver/context.h>
#include <opts/constantFolding.h>
#include <sema/sema.h>
#include <slir/gen/irgen.h>
#include <syntax/ast/ast.h>
#include <syntax/lexer.h>
#include <syntax/parser.h>

namespace assembler
{
Context::Context(std::string contents, DiagManager* diagManager, std::string outfile, outputBits ob,
                 outputFormat of, std::unordered_map<assembler::optimizations, bool> enabledOpts,
                 bool dumpAst, bool dumpIR)
{
    this->_contents    = contents;
    this->_diagManager = diagManager;
    this->_outfile     = outfile;
    this->_ob          = ob;
    this->_of          = of;
    this->_enabledOpts = enabledOpts;
    this->_dumpAst     = dumpAst;
    this->_dumpIR      = dumpIR;
}
Context::~Context() {}
void Context::start()
{
    Lexer*                     lexer         = new Lexer(this->_contents, this->_diagManager);
    Parser*                    parser        = new Parser(lexer, this->_diagManager);
    Ast*                       ast           = parser->getAst();
    opts::ConstantFoldingPass* constFoldPass = new opts::ConstantFoldingPass(
        this->_enabledOpts.at(assembler::optimizations::ConstantFolding));
    std::vector<std::pair<DiagLevel, std::string>> errors = constFoldPass->run(ast->getNodes());
    for (std::pair<DiagLevel, std::string> err : errors)
    {
        this->_diagManager->log(err.first, 0, "%s\n", err.second.c_str());
    }
    SemanticAnalyzer* sema = new SemanticAnalyzer(ast, this->_diagManager);
    sema->verify();
    if (this->_dumpAst)
    {
        ast->print();
    }
    ir::gen::IrGen* irGen   = new ir::gen::IrGen(this->_diagManager, ast, sema->getSymTable());
    ir::ir::Module* _module = irGen->genModule();
    _module->print();
    this->_diagManager->log(DiagLevel::ICE, 0,
                            "TODO: Go further trough the pipeline (Next is lirgen)\n");
}
}; // namespace assembler