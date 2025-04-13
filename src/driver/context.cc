#include <driver/context.h>
#include <sema/sema.h>
#include <syntax/ast/ast.h>
#include <syntax/lexer.h>
#include <syntax/parser.h>

namespace assembler
{
Context::Context(std::string contents, DiagManager* diagManager, std::string outfile, outputBits ob,
                 outputFormat of)
{
    this->_contents    = contents;
    this->_diagManager = diagManager;
    this->_outfile     = outfile;
    this->_ob          = ob;
    this->_of          = of;
}
Context::~Context() {}
void Context::start()
{
    Lexer*  lexer  = new Lexer(this->_contents, this->_diagManager);
    Parser* parser = new Parser(lexer, this->_diagManager);
    Ast*    ast    = parser->getAst();
    ast->print();
    SemanticAnalyzer* sema = new SemanticAnalyzer(ast, this->_diagManager);
    sema->verify();
    this->_diagManager->log(DiagLevel::ICE, 0,
                            "TODO: Go further trough the pipeline (Next is lirgen)\n");
}
}; // namespace assembler