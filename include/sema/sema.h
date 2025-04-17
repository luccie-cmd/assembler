#if !defined(_ASSEMBLER_SEMA_SEMA_H_)
#define _ASSEMBLER_SEMA_SEMA_H_
#include "symbol.h"

#include <cstdint>
#include <driver/diag.h>
#include <syntax/ast/ast.h>
#include <syntax/ast/decl.h>
#include <syntax/ast/expr.h>
#include <syntax/ast/inst.h>

namespace assembler
{
class SemanticAnalyzer
{
  public:
    SemanticAnalyzer(Ast* ast, DiagManager* diagManager);
    ~SemanticAnalyzer();
    void verify();

  private:
    void                                   verifyGlobalDecl(GlobalDeclarationNode* globalDeclNode);
    void                                   verifyExternDecl(ExternDeclarationNode* externDeclNode);
    void                                   verifySectionDecl(SectionDeclarationNode* sectionDecl);
    void                                   verifyLabelDecl(LabelDeclarationNode* labelDeclNode);
    void                                   verifyTypeDecl(TypeDeclarationNode* typeDeclNode);
    void                                   verifyArgDecl(ArgumentsDeclarationNode* argDeclNode);
    void                                   verifyInstruction(InstructionNode*& instNode);
    std::pair<ExpressionNodeType, uint8_t> verifyExpression(ExpressionNode* exprNode);
    void                                   verifyDeclaration(DeclarationNode* declNode);
    void                                   firstPass();
    void                                   secondPass();
    void                                   thirdPass();
    Ast*                                   _ast;
    DiagManager*                           _diagMngr;
    SymbolTable*                           _symTable;
    std::vector<AstNode*>                  _toHandleNodes;
};
}; // namespace assembler

#endif // _ASSEMBLER_SEMA_SEMA_H_
