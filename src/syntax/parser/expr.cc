#include <algorithm>
#include <syntax/ast/expr.h>
#include <syntax/parser.h>

namespace assembler
{
AstNode* Parser::parseMemoryExpression()
{
    this->consume();
    AstNode* base         = this->parsePrimaryExpression();
    AstNode* index        = nullptr;
    AstNode* scale        = nullptr;
    AstNode* displacement = nullptr;
    if (this->currentToken->get_type() == TokenType::PLUS)
    {
        this->consume();
        if (this->currentToken->get_type() == TokenType::IDENTIFIER)
        {
            index = this->parsePrimaryExpression();
        }
        if (this->currentToken->get_type() == TokenType::STAR)
        {
            this->consume();
            if (this->currentToken->get_type() == TokenType::LIT_NUMBER)
            {
                scale = this->parsePrimaryExpression();
            }
            else
            {
                this->_diagMngr->log(DiagLevel::ERROR, 0,
                                     "Expected a literal number but got `%s`\n",
                                     this->currentToken->get_value().c_str());
            }
        }
        if (this->currentToken->get_type() == TokenType::PLUS ||
            this->currentToken->get_type() == TokenType::MINUS ||
            this->currentToken->get_type() == TokenType::LIT_NUMBER)
        {
            if (this->currentToken->get_type() != TokenType::LIT_NUMBER)
            {
                this->consume();
            }
            displacement = this->parseExpression(0);
        }
    }
    this->expect(true, TokenType::CLOSEBRACKET);
    return new MemoryExpressionNode(base, index, scale, displacement);
}
AstNode* Parser::parsePrimaryExpression()
{
    switch (this->currentToken->get_type())
    {
    case TokenType::OPENBRACKET:
    {
        return this->parseMemoryExpression();
    }
    break;
    case TokenType::IDENTIFIER:
    {
        if (std::find(utils::registers.begin(), utils::registers.end(),
                      this->currentToken->get_value()) != utils::registers.end())
        {
            return new RegisterExpressionNode(this->consume());
        }
        else
        {
            return new VariableExpressionNode(this->consume());
        }
    }
    break;
    case TokenType::LIT_NUMBER:
    {
        return new ImmediateExpressionNode(this->consume());
    }
    break;
    case TokenType::LIT_STRING:
    {
        return new StringExpressionNode(this->consume());
    }
    break;
    default:
    {
        this->_diagMngr->log(DiagLevel::ICE, 0, "Unhandled expression token `%s`\n",
                             this->currentToken->get_value().c_str());
    }
    break;
    }
    return nullptr;
}
size_t Parser::getPrecedence(TokenType type)
{
    switch (type)
    {
    case TokenType::SLASH:
    case TokenType::STAR:
        return 2;
    case TokenType::MINUS:
    case TokenType::PLUS:
        return 1;
    default:
    {
        this->_diagMngr->log(DiagLevel::ICE, 0, "Invalid precedence call with type `%lu`\n",
                             (size_t)type);
    }
    break;
    }
    return -1;
}
AstNode* Parser::parseExpression(size_t min_prec)
{
    if (this->currentToken->get_type() == TokenType::QWORD)
    {
        return this->parseExpressionSized(64);
    }
    else if (this->currentToken->get_type() == TokenType::DWORD)
    {
        return this->parseExpressionSized(32);
    }
    else if (this->currentToken->get_type() == TokenType::WORD)
    {
        return this->parseExpressionSized(16);
    }
    else if (this->currentToken->get_type() == TokenType::BYTE)
    {
        return this->parseExpressionSized(8);
    }
    AstNode* lhs = this->parsePrimaryExpression();
    while (this->currentToken->get_type() == TokenType::PLUS ||
           this->currentToken->get_type() == TokenType::MINUS ||
           this->currentToken->get_type() == TokenType::STAR ||
           this->currentToken->get_type() == TokenType::SLASH)
    {
        Token* operation  = this->currentToken;
        size_t precedence = this->getPrecedence(this->currentToken->get_type());
        if (precedence < min_prec)
        {
            break;
        }
        this->consume();
        AstNode* rhs = this->parseExpression(precedence + 1);
        lhs          = new BinaryExpressionNode(lhs, operation, rhs);
    }
    return lhs;
}
AstNode* Parser::parseExpressionSized(size_t size)
{
    this->consume();
    return new SizedExpressionNode(size,
                                   reinterpret_cast<ExpressionNode*>(this->parseExpression(0)));
}
} // namespace assembler