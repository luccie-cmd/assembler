#include <syntax/lexer.h>
#include <utility>
#include <vector>

bool IsStart(char c)
{
    return std::isalpha(c) || c == '_' || c == '.';
}

/// Check if a character is allowed in an identifier.
bool IsContinue(char c)
{
    return IsStart(c) || isdigit(c);
}

bool IsBinary(char c)
{
    return c == '0' || c == '1';
}
bool IsDecimal(char c)
{
    return c >= '0' && c <= '9';
}
bool IsOctal(char c)
{
    return c >= '0' && c <= '7';
}
bool IsHex(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

namespace assembler
{
void Lexer::removeWhitespace()
{
    while (1)
    {
        if (this->c == ';')
        {
            while (this->c != '\n' && !this->at_eof())
            {
                this->next_char();
            }
        }
        if (!isspace(this->c))
        {
            return;
        }
        this->next_char();
    }
}
Token* Lexer::nextToken()
{
    this->removeWhitespace();
    // First going to check for the EOF
    while (this->c == ';' && !this->at_eof())
    {
        while (this->c != '\n' && !this->at_eof())
        {
            this->next_char();
        }
        this->removeWhitespace();
    }
    if (this->at_eof())
    {
        return new Token("EOF", TokenType::TEOF);
    }
    Token* ret = new Token("Invalid", TokenType::INVALID);
    switch (this->c)
    {
    case '@':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case ',':
    case '*':
    case '/':
    case '#':
    case '-':
    case ':':
    case '<':
    case '>':
    case '=':
    case '+':
    {
        ret->set_type(static_cast<TokenType>(this->c));
        ret->set_value(std::string(1, this->c));
        this->next_char();
    }
    break;
    case '\"':
    {
        Token* string = this->lexString();
        ret->set_type(string->get_type());
        ret->set_value(string->get_value());
    }
    break;
    default:
    {
        if (IsStart(this->c))
        {
            Token* identifer_or_keyword = this->lexIdentifierOrKeyword();
            ret->set_type(identifer_or_keyword->get_type());
            ret->set_value(identifer_or_keyword->get_value());
        }
        else if (IsDecimal(this->c))
        {
            Token* number = this->lexNumber();
            ret->set_type(number->get_type());
            ret->set_value(number->get_value());
        }
        else
        {
            this->diagMngr->log(DiagLevel::ERROR, this->curr,
                                "Invalid character found with the value of `%c`\n", this->c);
        }
    }
    break;
    }
    return ret;
}
std::vector<std::pair<std::string, TokenType>> keywords = {
    {"global", TokenType::GLOBAL},   {"extern", TokenType::EXTERN}, {"type", TokenType::TYPE},
    {"section", TokenType::SECTION}, {"qword", TokenType::QWORD},
};
TokenType getTokenTypeIdentifierKeyword(std::string buffer)
{
    for (std::pair<std::string, TokenType> keyword : keywords)
    {
        if (buffer == keyword.first)
        {
            return keyword.second;
        }
    }
    return TokenType::IDENTIFIER;
}
Token* Lexer::lexIdentifierOrKeyword()
{
    std::string buffer(1, this->c);
    this->next_char();
    while (IsContinue(this->c))
    {
        buffer.push_back(this->c);
        this->next_char();
    }
    TokenType tt = getTokenTypeIdentifierKeyword(buffer);
    return new Token(buffer, tt);
}
Token* Lexer::lexString()
{
    this->next_char();
    std::string buffer(1, this->c);
    this->next_char();
    while (this->c != '\"')
    {
        buffer.push_back(this->c);
        this->next_char();
        if (this->at_eof())
        {
            this->diagMngr->log(DiagLevel::ERROR, this->curr, "Missing terminating \" character\n");
        }
    }
    this->next_char(); // Skip the "
    TokenType tt = TokenType::LIT_STRING;
    return new Token(buffer, tt);
}
Token* Lexer::lexNumber()
{
    std::string buffer(1, this->c);
    this->next_char();
    while (IsDecimal(this->c))
    {
        buffer.push_back(this->c);
        this->next_char();
    }
    TokenType tt = TokenType::LIT_NUMBER;
    return new Token(buffer, tt);
}
std::vector<Token*> Lexer::lexAllTokens()
{
    std::vector<Token*> tokens;
    Token*              token = this->nextToken();
    while (token->get_type() != TokenType::TEOF)
    {
        tokens.push_back(token);
        if (token->get_type() == TokenType::INVALID)
        {
            break;
        }
        token = this->nextToken();
    }
    // Push EOF onto the tokens
    tokens.push_back(token);
    return tokens;
}
}; // namespace assembler