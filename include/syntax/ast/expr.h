#if !defined(_ASSEMBLER_SYNTAX_AST_EXPR_H_)
#define _ASSEMBLER_SYNTAX_AST_EXPR_H_
#include "node.h"

#include <syntax/token.h>

namespace assembler
{
enum struct ExpressionNodeType
{
    Register,
    Memory,
    Immediate,
    String,
    Binary,
    Variable,
    Sized,
};
class ExpressionNode : public AstNode
{
  public:
    ExpressionNode(ExpressionNodeType _type);
    virtual ~ExpressionNode()                    = 0;
    virtual void       print(size_t spaceOffset) = 0;
    ExpressionNodeType getExprType();

  protected:
    ExpressionNodeType expressionType;
};
class RegisterExpressionNode : public ExpressionNode
{
  public:
    RegisterExpressionNode(Token* _register);
    ~RegisterExpressionNode();
    void   print(size_t spaceOffset);
    Token* getRegister();

  private:
    Token* _register;
};
class MemoryExpressionNode : public ExpressionNode
{
  public:
    MemoryExpressionNode(AstNode* base, AstNode* index, AstNode* scale, AstNode* displacement);
    ~MemoryExpressionNode();
    void     print(size_t spaceOffset);
    AstNode*& getBase();
    AstNode*& getIndex();
    AstNode*& getScale();
    AstNode*& getDisplacement();

  private:
    AstNode* _base;
    AstNode* _index;
    AstNode* _scale;
    AstNode* _displacement;
};
class ImmediateExpressionNode : public ExpressionNode
{
  public:
    ImmediateExpressionNode(Token* value);
    ~ImmediateExpressionNode();
    void   print(size_t spaceOffset);
    Token* getValue();

  private:
    Token* _value;
};
class BinaryExpressionNode : public ExpressionNode
{
  public:
    BinaryExpressionNode(AstNode* lhs, Token* operation, AstNode* rhs);
    ~BinaryExpressionNode();
    void print(size_t spaceOffset);
    AstNode* getLhs();
    Token*   getOperation();
    AstNode* getRhs();

  private:
    AstNode* _lhs;
    Token*   _operation;
    AstNode* _rhs;
};
class VariableExpressionNode : public ExpressionNode
{
  public:
    VariableExpressionNode(Token* value);
    ~VariableExpressionNode();
    void print(size_t spaceOffset);
    Token* getName();

  private:
    Token* _value;
};
class StringExpressionNode : public ExpressionNode
{
  public:
    StringExpressionNode(Token* value);
    ~StringExpressionNode();
    void print(size_t spaceOffset);

  private:
    Token* _value;
};
class SizedExpressionNode : public ExpressionNode
{
  public:
    SizedExpressionNode(size_t size, ExpressionNode* expr);
    ~SizedExpressionNode();
    void print(size_t spaceOffset);
    size_t          getSize();
    ExpressionNode* getExpr();

  private:
    size_t          size;
    ExpressionNode* expr;
};
} // namespace assembler

#endif // _ASSEMBLER_SYNTAX_AST_EXPR_H_
