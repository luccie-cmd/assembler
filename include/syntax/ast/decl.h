#if !defined(_ASSEMBLER_SYNTAX_AST_DECL_H_)
#define _ASSEMBLER_SYNTAX_AST_DECL_H_
#include "node.h"

#include <syntax/token.h>

namespace assembler
{
enum struct DeclarationNodeType
{
    Global,
    Extern,
    Type,
    Section,
    Label,
};
class DeclarationNode : public AstNode
{
  public:
    DeclarationNode(DeclarationNodeType _type);
    virtual ~DeclarationNode()                    = 0;
    virtual void        print(size_t spaceOffset) = 0;
    DeclarationNodeType getDeclType();

  protected:
    DeclarationNodeType declarationNodeType;
};
class GlobalDeclarationNode : public DeclarationNode
{
  public:
    GlobalDeclarationNode(Token* name);
    ~GlobalDeclarationNode();
    void   print(size_t spaceOffset);
    Token* getName();

  private:
    Token* _name;
};
class ExternDeclarationNode : public DeclarationNode
{
  public:
    ExternDeclarationNode(Token* name);
    ~ExternDeclarationNode();
    void   print(size_t spaceOffset);
    Token* getName();

  private:
    Token* _name;
};
class TypeDeclarationNode : public DeclarationNode
{
  public:
    TypeDeclarationNode(Token* name, Token* type);
    ~TypeDeclarationNode();
    void   print(size_t spaceOffset);
    Token* getName();
    Token* getType();

  private:
    Token* _name;
    Token* _type;
};
class SectionDeclarationNode : public DeclarationNode
{
  public:
    SectionDeclarationNode(Token* name);
    ~SectionDeclarationNode();
    void   print(size_t spaceOffset);
    Token* getName();

  private:
    Token* _name;
};
class LabelDeclarationNode : public DeclarationNode
{
  public:
    LabelDeclarationNode(Token* name);
    ~LabelDeclarationNode();
    void   print(size_t spaceOffset);
    Token* getName();

  private:
    Token* _name;
};
}; // namespace assembler

#endif // _ASSEMBLER_SYNTAX_AST_DECL_H_
