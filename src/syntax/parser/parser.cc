#include <algorithm>
#include <syntax/parser.h>

namespace assembler
{
Parser::Parser(Lexer* lexer, DiagManager* diagMngr)
{
    this->_lexer    = lexer;
    this->_diagMngr = diagMngr;
    this->consume();
}
Parser::~Parser() {}
Token* Parser::consume()
{
    Token* current = this->currentToken;
    if (this->buffer.empty())
    {
        this->currentToken = this->_lexer->nextToken();
    }
    else
    {
        this->currentToken = this->buffer.front();
        this->buffer.erase(this->buffer.begin());
    }
    return current;
}
Token* Parser::peek(size_t lookahead)
{
    while (this->buffer.size() <= lookahead)
    {
        Token* nextToken = this->_lexer->nextToken();
        if (nextToken == nullptr)
        {
            this->_diagMngr->log(DiagLevel::ICE, 0, "Attempted invalid lookahead\n");
            return nullptr;
        }
        this->buffer.push_back(nextToken);
    }
    return this->buffer[lookahead];
}
Token* Parser::expect(bool consume, TokenType expected)
{
    if (this->currentToken->get_type() == expected)
    {
        Token* ret = this->currentToken;
        if (consume)
        {
            ret = this->consume();
        }
        return ret;
    }
    this->_diagMngr->log(DiagLevel::ERROR, 0, "Expected `%lu` but got `%s`\n", (size_t)expected,
                         this->currentToken->get_value().c_str());
    std::exit(1);
}
Token* Parser::expect(bool consume, std::vector<TokenType> expecteds)
{
    for (TokenType expected : expecteds)
    {
        if (this->currentToken->get_type() == expected)
        {
            Token* ret = this->currentToken;
            if (consume)
            {
                ret = this->consume();
            }
            return ret;
        }
    }
    this->_diagMngr->log(DiagLevel::NOTE, 0, "Expected ");
    for (size_t i = 0; i < expecteds.size(); ++i)
    {
        std::printf("`%lu` %s", (size_t)expecteds.at(i), i == expecteds.size() - 1 ? "" : "or ");
    }
    std::printf("but got `%s`\n", this->currentToken->get_value().c_str());
    std::exit(1);
}
AstNode* Parser::parseNode()
{
    switch (this->currentToken->get_type())
    {
    case TokenType::TYPE:
    case TokenType::GLOBAL:
    case TokenType::EXTERN:
    case TokenType::SECTION:
    {
        return this->parseDeclaration();
    }
    break;
    case TokenType::LIT_NUMBER:
    {
        return this->parseExpression(0);
    }
    break;
    // Parsing that needs to check what's behind it
    case TokenType::IDENTIFIER:
    {
        Token* lookaheadToken = this->peek(0);
        switch (lookaheadToken->get_type())
        {
        case TokenType::COLON:
        {
            return this->parseLabelDecl();
        }
        break;
        case TokenType::PLUS:
        case TokenType::MINUS:
        case TokenType::STAR:
        case TokenType::SLASH:
        {
            return this->parseExpression(0);
        }
        break;
        default:
        {
            if (this->currentToken->get_value() == "db" ||
                this->currentToken->get_value() == "dw" ||
                this->currentToken->get_value() == "dd" || this->currentToken->get_value() == "dq")
            {
                return this->parseDirectBytes();
            }
            return this->parseInstruction();
        }
        break;
        }
    }
    break;
    case TokenType::LIT_STRING:
    default:
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0, "Unexpected token `%s`\n",
                             this->currentToken->get_value().c_str());
    }
    }
    return nullptr;
}
Ast* Parser::getAst()
{
    std::vector<AstNode*> nodes;
    while (this->currentToken->get_type() != TokenType::TEOF)
    {
        AstNode* node = this->parseNode();
        node->print(0);
        nodes.push_back(node);
    }
    return new Ast(nodes);
}
}; // namespace assembler