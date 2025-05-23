#include <syntax/ast/decl.h>
#include <syntax/parser.h>

namespace assembler
{
AstNode* Parser::parseGlobalDecl()
{
    return new GlobalDeclarationNode(this->expect(true, TokenType::IDENTIFIER));
}
AstNode* Parser::parseExternDecl()
{
    return new ExternDeclarationNode(this->expect(true, TokenType::IDENTIFIER));
}
AstNode* Parser::parseTypeDeclaration()
{
    Token* name = this->expect(true, TokenType::IDENTIFIER);
    this->expect(true, TokenType::COMMA);
    Token* type = this->expect(true, TokenType::IDENTIFIER);
    return new TypeDeclarationNode(name, type);
}
AstNode* Parser::parseSectionDecl()
{
    return new SectionDeclarationNode(this->expect(true, TokenType::IDENTIFIER));
}
AstNode* Parser::parseArgumentsDecl()
{
    Token* name = this->expect(true, TokenType::IDENTIFIER);
    this->expect(true, TokenType::COMMA);
    Token* argumentCount = this->expect(true, TokenType::LIT_NUMBER);
    return new ArgumentsDeclarationNode(name, argumentCount);
}
AstNode* Parser::parseDeclaration()
{
    switch (this->currentToken->get_type())
    {
    case TokenType::TYPE:
    {
        this->consume();
        return this->parseTypeDeclaration();
    }
    break;
    case TokenType::SECTION:
    {
        this->consume();
        return this->parseSectionDecl();
    }
    break;
    case TokenType::GLOBAL:
    {
        this->consume();
        return this->parseGlobalDecl();
    }
    break;
    case TokenType::EXTERN:
    {
        this->consume();
        return this->parseExternDecl();
    }
    break;
    case TokenType::ARGUMENTS:
    {
        this->consume();
        return this->parseArgumentsDecl();
    }
    break;
    default:
    {
        this->_diagMngr->log(DiagLevel::ICE, 0, "Invalid declaration parsing of `%s`\n",
                             this->currentToken->get_value().c_str());
    }
    break;
    }
    return nullptr;
}
AstNode* Parser::parseLabelDecl()
{
    Token* identifier = this->expect(true, TokenType::IDENTIFIER);
    if (this->currentLabelDefinition == nullptr && !identifier->get_value().starts_with("."))
    {
        this->currentLabelDefinition = identifier;
    }
    else
    {
        std::string newName = identifier->get_value();
        if (!identifier->get_value().starts_with("."))
        {
            this->currentLabelDefinition = nullptr;
        }
        else
        {
            newName = this->currentLabelDefinition->get_value() + identifier->get_value();
        }
        identifier->set_value(newName);
    }
    this->expect(true, TokenType::COLON);
    return new LabelDeclarationNode(identifier);
}
} // namespace assembler