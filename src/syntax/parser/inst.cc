#include <algorithm>
#include <syntax/ast/inst.h>
#include <syntax/parser.h>

namespace assembler
{
AstNode* Parser::parseInstruction()
{
    Token* mnemonic = this->consume();
    if (std::find(utils::opcodes.begin(), utils::opcodes.end(), mnemonic->get_value()) ==
        utils::opcodes.end())
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0, "Expected instruction but got `%s`\n",
                             mnemonic->get_value().c_str());
    }
    std::vector<AstNode*> arguments;
    if (utils::takesArgument(mnemonic->get_value()))
    {
        while (1)
        {
            arguments.push_back(this->parseExpression(0));
            if (this->currentToken->get_type() != TokenType::COMMA)
            {
                break;
            }
            this->expect(true, TokenType::COMMA);
        }
    }
    return new InstructionNode(mnemonic, arguments);
}
} // namespace assembler