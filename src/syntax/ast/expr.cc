#include <syntax/ast/expr.h>

namespace assembler
{
ExpressionNode::ExpressionNode(ExpressionNodeType _type) : AstNode(AstNodeType::Expression)
{
    this->expressionType = _type;
}
ExpressionNode::~ExpressionNode() {}
ExpressionNodeType ExpressionNode::getExprType()
{
    return this->expressionType;
}
RegisterExpressionNode::RegisterExpressionNode(Token* _register)
    : ExpressionNode(ExpressionNodeType::Register)
{
    this->_register = _register;
}
RegisterExpressionNode::~RegisterExpressionNode() {}
void RegisterExpressionNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Expression\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Register: `%s`\n", this->_register->get_value().c_str());
}
Token* RegisterExpressionNode::getRegister()
{
    return this->_register;
}
VariableExpressionNode::VariableExpressionNode(Token* variable)
    : ExpressionNode(ExpressionNodeType::Variable)
{
    this->_value = variable;
}
VariableExpressionNode::~VariableExpressionNode() {}
void VariableExpressionNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Expression\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Variable: `%s`\n", this->_value->get_value().c_str());
}
Token* VariableExpressionNode::getName()
{
    return this->_value;
}
ImmediateExpressionNode::ImmediateExpressionNode(Token* value)
    : ExpressionNode(ExpressionNodeType::Immediate)
{
    this->_value = value;
}
ImmediateExpressionNode::~ImmediateExpressionNode() {}
void ImmediateExpressionNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Expression\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Immediate: `%s`\n", this->_value->get_value().c_str());
}
Token* ImmediateExpressionNode::getValue()
{
    return this->_value;
}
BinaryExpressionNode::BinaryExpressionNode(AstNode* lhs, Token* operation, AstNode* rhs)
    : ExpressionNode(ExpressionNodeType::Binary)
{
    this->_lhs       = lhs;
    this->_operation = operation;
    this->_rhs       = rhs;
}
BinaryExpressionNode::~BinaryExpressionNode() {}
void BinaryExpressionNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Expression\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- LHS:\n");
    this->_lhs->print(spacing + (SPACING_SIZE * 2));
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Operation: `%s`\n", this->_operation->get_value().c_str());
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- RHS:\n");
    this->_rhs->print(spacing + (SPACING_SIZE * 2));
}
MemoryExpressionNode::MemoryExpressionNode(AstNode* base, AstNode* index, AstNode* scale,
                                           AstNode* displacement)
    : ExpressionNode(ExpressionNodeType::Memory)
{
    this->_base         = base;
    this->_index        = index;
    this->_scale        = scale;
    this->_displacement = displacement;
}
MemoryExpressionNode::~MemoryExpressionNode() {}
void MemoryExpressionNode::print(size_t spacing)
{
    for (size_t i = 0; i < spacing; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Expression\n");
    for (size_t i = 0; i < spacing + SPACING_SIZE; ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Memory\n");
    for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
    {
        std::putchar(' ');
    }
    std::printf("- Base:\n");
    this->_base->print(spacing + (SPACING_SIZE * 3));
    if (this->_index)
    {
        for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
        {
            std::putchar(' ');
        }
        std::printf("- Index:\n");
        this->_index->print(spacing + (SPACING_SIZE * 3));
    }
    if (this->_scale)
    {
        for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
        {
            std::putchar(' ');
        }
        std::printf("- Scale:\n");
        this->_scale->print(spacing + (SPACING_SIZE * 3));
    }
    if (this->_displacement)
    {
        for (size_t i = 0; i < spacing + (SPACING_SIZE * 2); ++i)
        {
            std::putchar(' ');
        }
        std::printf("- Displacement:\n");
        this->_displacement->print(spacing + (SPACING_SIZE * 3));
    }
}
AstNode* MemoryExpressionNode::getBase()
{
    return this->_base;
}
AstNode* MemoryExpressionNode::getIndex()
{
    return this->_index;
}
AstNode* MemoryExpressionNode::getScale()
{
    return this->_scale;
}
AstNode* MemoryExpressionNode::getDisplacement()
{
    return this->_displacement;
}
StringExpressionNode::StringExpressionNode(Token* value)
    : ExpressionNode(ExpressionNodeType::String)
{
    this->_value = value;
}
StringExpressionNode::~StringExpressionNode() {}
void StringExpressionNode::print(size_t spacing)
{
    (void)spacing;
    std::printf("TODO: Print string expression nodes\n");
}
SizedExpressionNode::SizedExpressionNode(size_t size, ExpressionNode* expr)
    : ExpressionNode(ExpressionNodeType::Sized)
{
    this->expr = expr;
    this->size = size;
}
SizedExpressionNode::~SizedExpressionNode() {}
void SizedExpressionNode::print(size_t spacing)
{
    (void)spacing;
    std::printf("TODO: Print sized expression nodes\n");
}
size_t SizedExpressionNode::getSize()
{
    return this->size * 8;
}
ExpressionNode* SizedExpressionNode::getExpr()
{
    return this->expr;
}
}; // namespace assembler