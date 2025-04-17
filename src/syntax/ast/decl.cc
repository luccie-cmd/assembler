#include <syntax/ast/decl.h>

namespace assembler
{
DeclarationNode::DeclarationNode(DeclarationNodeType type) : AstNode(AstNodeType::Declaration)
{
    this->declarationNodeType = type;
}
DeclarationNode::~DeclarationNode() {}
DeclarationNodeType DeclarationNode::getDeclType()
{
    return this->declarationNodeType;
}
GlobalDeclarationNode::GlobalDeclarationNode(Token* name)
    : DeclarationNode(DeclarationNodeType::Global)
{
    this->_name = name;
}
GlobalDeclarationNode::~GlobalDeclarationNode() {}
Token* GlobalDeclarationNode::getName()
{
    return this->_name;
}
void GlobalDeclarationNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Declaration\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Global\n");
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Name: `%s`\n", this->_name->get_value().c_str());
}
ExternDeclarationNode::ExternDeclarationNode(Token* name)
    : DeclarationNode(DeclarationNodeType::Extern)
{
    this->_name = name;
}
ExternDeclarationNode::~ExternDeclarationNode() {}
void ExternDeclarationNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Declaration\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Extern\n");
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Name: `%s`\n", this->_name->get_value().c_str());
}
Token* ExternDeclarationNode::getName()
{
    return this->_name;
}
SectionDeclarationNode::SectionDeclarationNode(Token* name)
    : DeclarationNode(DeclarationNodeType::Section)
{
    this->_name = name;
}
SectionDeclarationNode::~SectionDeclarationNode() {}
void SectionDeclarationNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Declaration\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Section\n");
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Name: `%s`\n", this->_name->get_value().c_str());
}
Token* SectionDeclarationNode::getName()
{
    return this->_name;
}
LabelDeclarationNode::LabelDeclarationNode(Token* name)
    : DeclarationNode(DeclarationNodeType::Label)
{
    this->_name = name;
}
LabelDeclarationNode::~LabelDeclarationNode() {}
void LabelDeclarationNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Declaration\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Label\n");
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Name: `%s`\n", this->_name->get_value().c_str());
}
Token* LabelDeclarationNode::getName()
{
    return this->_name;
}
TypeDeclarationNode::TypeDeclarationNode(Token* name, Token* type)
    : DeclarationNode(DeclarationNodeType::Type)
{
    this->_name = name;
    this->_type = type;
}
TypeDeclarationNode::~TypeDeclarationNode() {}
void TypeDeclarationNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Declaration\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Type\n");
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Name: `%s`\n", this->_name->get_value().c_str());
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Type: `%s`\n", this->_type->get_value().c_str());
}
Token* TypeDeclarationNode::getName()
{
    return this->_name;
}
Token* TypeDeclarationNode::getType()
{
    return this->_type;
}
ArgumentsDeclarationNode::ArgumentsDeclarationNode(Token* name, Token* count)
    : DeclarationNode(DeclarationNodeType::Arguments)
{
    this->_name  = name;
    this->_count = count;
}
ArgumentsDeclarationNode::~ArgumentsDeclarationNode() {}
void ArgumentsDeclarationNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Declaration\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Type\n");
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Name: `%s`\n", this->_name->get_value().c_str());
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Count: %lu\n", std::stoul(this->_count->get_value()));
}
Token* ArgumentsDeclarationNode::getName()
{
    return this->_name;
}
Token* ArgumentsDeclarationNode::getCount()
{
    return this->_count;
}
}; // namespace assembler