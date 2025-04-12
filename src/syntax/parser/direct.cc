#include <syntax/ast/direct.h>
#include <syntax/parser.h>

namespace assembler
{
AstNode* Parser::parseDirectBytes()
{
    uint8_t bytesPerArg = 0;
    switch (this->currentToken->get_value().at(this->currentToken->get_value().size() - 1))
    {
    case 'b':
        bytesPerArg = 1;
        break;
    case 'w':
        bytesPerArg = 2;
        break;
    case 'd':
        bytesPerArg = 4;
        break;
    case 'q':
        bytesPerArg = 8;
        break;
    default:
    {
        this->_diagMngr->log(DiagLevel::ICE, 0, "Unexpected size specifier `%s`\n",
                             this->currentToken->get_value().c_str());
    }
    break;
    }
    this->consume();
    std::vector<AstNode*> arguments;
    while (1)
    {
        arguments.push_back(this->parseExpression(0));
        if (this->currentToken->get_type() != TokenType::COMMA)
        {
            break;
        }
        this->consume();
    }
    return new DirectBytesNode(bytesPerArg, arguments);
}
} // namespace assembler