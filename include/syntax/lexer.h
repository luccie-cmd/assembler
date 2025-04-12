#if !defined(_ASSEMBLER_SYNTAX_LEXER_H_)
#define _ASSEMBLER_SYNTAX_LEXER_H_
#include "token.h"

#include <cstdint>
#include <driver/diag.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace assembler
{
class Lexer
{
  public:
    Lexer(std::string data, DiagManager* diagMngr)
    {
        if (data.size() == 0)
        {
            diagMngr->log(DiagLevel::ERROR, 0, "We didn't get any data, so no parsing\n");
            return;
        }
        this->data     = data;
        this->curr     = 0;
        this->c        = this->data.at(this->curr++);
        this->diagMngr = diagMngr;
    }
    void                removeWhitespace();
    Token*              nextToken();
    Token*              lexIdentifierOrKeyword();
    Token*              lexString();
    Token*              lexNumber();
    std::vector<Token*> lexAllTokens();

  private:
    std::string   data;
    std::uint64_t curr;
    DiagManager*  diagMngr;
    char          c;
    inline bool   at_eof()
    {
        return (this->c == '\0');
    }
    inline void next_char()
    {
        // if we can't increment count anymore make the current character NULL
        if (this->curr < this->data.size())
        {
            this->c = this->data.at(this->curr++);
        }
        else
        {
            this->c = '\0';
        }
    }
};
} // namespace assembler

#endif // _ASSEMBLER_SYNTAX_LEXER_H_
