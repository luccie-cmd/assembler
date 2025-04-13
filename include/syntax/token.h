#if !defined(_ASSEMBLER_SYNTAX_TOKEN_H_)
#define _ASSEMBLER_SYNTAX_TOKEN_H_
#include <string>

namespace assembler
{
enum struct TokenType
{
    INVALID = 0,
    TEOF    = 1, // Why not just EOF well C++ doesn't like that

    OPENPAREN    = '(',
    CLOSEPAREN   = ')',
    OPENBRACKET  = '[',
    CLOSEBRACKET = ']',
    OPENBRACE    = '{',
    CLOSEBRACE   = '}',
    SEMICOLON    = ';',
    COLON        = ':',
    AT           = '@',
    COMMA        = ',',
    LESS         = '<',
    GREATER      = '>',
    EQUAL        = '=',
    STAR         = '*',
    PLUS         = '+',
    MINUS        = '-',
    SLASH        = '/',
    HASHTAG      = '#',

    __MULTIBYTE_START = 255,
    IDENTIFIER,
    LIT_STRING,
    LIT_NUMBER,

    __KEYWORDS_START = 300,
    GLOBAL,
    EXTERN,
    TYPE,
    SECTION,
    QWORD,
    DWORD,
    WORD,
    BYTE,
};
class Token
{
  private:
    std::string value;
    TokenType   type;

  public:
    Token(std::string value, TokenType type)
    {
        this->value = value;
        this->type  = type;
    }
    std::string& get_value()
    {
        return this->value;
    };
    TokenType get_type()
    {
        return this->type;
    };
    void set_value(std::string value)
    {
        this->value = value;
    }
    void set_type(TokenType type)
    {
        this->type = type;
    }
};
} // namespace assembler

#endif // _ASSEMBLER_SYNTAX_TOKEN_H_
