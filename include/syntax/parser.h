#if !defined(_ASSEMBLER_SYNTAX_PARSER_H_)
#define _ASSEMBLER_SYNTAX_PARSER_H_
#include "ast/ast.h"
#include "lexer.h"

namespace utils
{
extern std::vector<std::string> opcodes;
extern std::vector<std::string> registers;
bool                            takesArgument(std::string opcode);
} // namespace utils

namespace assembler
{
class Parser
{
  public:
    Parser(Lexer* lexer, DiagManager* diagMngr);
    ~Parser();
    Ast* getAst();

  private:
    Token*              consume();
    Token*              peek(size_t lookAhead);
    Token*              expect(bool consume, TokenType expected);
    Token*              expect(bool consume, std::vector<TokenType> expected);
    size_t              getPrecedence(TokenType type);
    AstNode*            parseDirectBytes();
    AstNode*            parseMemoryExpression();
    AstNode*            parsePrimaryExpression();
    AstNode*            parseExpression(size_t min_precedence);
    AstNode*            parseExpressionSized(size_t size);
    AstNode*            parseTypeDeclaration();
    AstNode*            parseGlobalDecl();
    AstNode*            parseExternDecl();
    AstNode*            parseSectionDecl();
    AstNode*            parseLabelDecl();
    AstNode*            parseInstruction();
    AstNode*            parseDeclaration();
    AstNode*            parseNode();
    Lexer*              _lexer;
    Token*              currentToken;
    DiagManager*        _diagMngr;
    std::vector<Token*> buffer;
};
} // namespace assembler

#endif // _ASSEMBLER_SYNTAX_PARSER_H_
