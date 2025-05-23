#include <algorithm>
#include <cstdint>
#include <ranges>
#include <sema/sema.h>
#include <syntax/ast/decl.h>
#include <syntax/ast/expr.h>
#include <syntax/ast/inst.h>

namespace utils
{
extern std::vector<std::string> registers;
uint8_t                         getRegisterSize(std::string reg);
bool                            getBranchInstruction(std::string mnemonic);
} // namespace utils

namespace assembler
{
SemanticAnalyzer::SemanticAnalyzer(Ast* inAst, DiagManager* diagManager)
{
    this->_ast      = inAst;
    this->_diagMngr = diagManager;
    this->_symTable = new SymbolTable();
}
SemanticAnalyzer::~SemanticAnalyzer()
{
    delete this->_symTable;
}
SymbolTable* SemanticAnalyzer::getSymTable()
{
    return this->_symTable;
}
std::vector<std::pair<std::string, SymbolKind>> stringSymPairs = {
    {"function", SymbolKind::Function}, {"object", SymbolKind::Object},
    {"section", SymbolKind::Section},   {"unknown", SymbolKind::Unknown},
    {"unset", SymbolKind::Unset},
};
static SymbolKind stringToSymKind(std::string data)
{
    for (std::pair<std::string, SymbolKind> stringSymPair : stringSymPairs)
    {
        if (stringSymPair.first == data)
        {
            return stringSymPair.second;
        }
    }
    return SymbolKind::Unknown;
}
static std::string symKindToString(SymbolKind kind)
{
    for (std::pair<std::string, SymbolKind> stringSymPair : stringSymPairs)
    {
        if (stringSymPair.second == kind)
        {
            return stringSymPair.first;
        }
    }
    return std::string("Invalid");
}
void SemanticAnalyzer::verifyGlobalDecl(GlobalDeclarationNode* globalDeclNode)
{
    Symbol* checkSymbol = this->_symTable->getSymbolByName(globalDeclNode->getName()->get_value());
    if (checkSymbol != nullptr)
    {
        if (checkSymbol->getSymbolBind() == SymbolBinding::Extern)
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Dubious declaration of a global as symbol `%s` has "
                                 "already been defined as external\n",
                                 checkSymbol->getName().c_str());
        }
        else
        {
            this->_diagMngr->log(
                DiagLevel::WARNING, 0,
                "Attempted to add symbol `%s` twice with the same binding of global\n",
                globalDeclNode->getName()->get_value().c_str());
        }
    }
    Symbol* symbol =
        new Symbol(SymbolBinding::Global, globalDeclNode->getName()->get_value(), false);
    this->_symTable->pushSymbol(symbol);
}
void SemanticAnalyzer::verifyExternDecl(ExternDeclarationNode* externDeclNode)
{
    Symbol* checkSymbol = this->_symTable->getSymbolByName(externDeclNode->getName()->get_value());
    if (checkSymbol != nullptr)
    {
        if (checkSymbol->getSymbolBind() == SymbolBinding::Global)
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Dubious declaration of an external as symbol `%s` "
                                 "has already been defined as global\n",
                                 checkSymbol->getName().c_str());
        }
        else
        {
            this->_diagMngr->log(DiagLevel::WARNING, 0,
                                 "Attempted to add symbol `%s` twice with the same "
                                 "binding of external\n",
                                 externDeclNode->getName()->get_value().c_str());
        }
    }
    Symbol* symbol =
        new Symbol(SymbolBinding::Extern, externDeclNode->getName()->get_value(), false);
    this->_symTable->pushSymbol(symbol);
}
void SemanticAnalyzer::verifySectionDecl(SectionDeclarationNode* sectionDecl)
{
    Symbol* checkSymbol = this->_symTable->getSymbolByName(sectionDecl->getName()->get_value());
    if (checkSymbol != nullptr)
    {
        if (checkSymbol->getSymbolKind() == SymbolKind::Section)
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Redefinition of an already existing section called `%s`\n",
                                 checkSymbol->getName().c_str());
        }
        else
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Redefinition of an already existing symbol with a "
                                 "type of `%s` called `%s`\n",
                                 symKindToString(checkSymbol->getSymbolKind()).c_str(),
                                 checkSymbol->getName().c_str());
        }
    }
    Symbol* newSymbol =
        new Symbol(SymbolBinding::Local, sectionDecl->getName()->get_value().c_str(), false);
    newSymbol->setSymbolKind(SymbolKind::Section);
    this->_symTable->pushSymbol(newSymbol);
}
std::string getParent(const std::string input)
{
    auto dot_pos = input.find('.');
    if (dot_pos == std::string::npos)
    {
        return ""; // No parent
    }
    return input.substr(0, dot_pos);
}
void SemanticAnalyzer::verifyLabelDecl(LabelDeclarationNode* labelDeclNode)
{
    Symbol* checkSymbol = this->_symTable->getSymbolByName(labelDeclNode->getName()->get_value());
    if (checkSymbol != nullptr)
    {
        if (checkSymbol->getIsDefinedByLabel())
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Redefinition of an already existing label `%s`\n",
                                 checkSymbol->getName().c_str());
        }
    }
    if (checkSymbol == nullptr)
    {
        Symbol* newSymbol =
            new Symbol(SymbolBinding::Local, labelDeclNode->getName()->get_value().c_str(), true);
        newSymbol->setSymbolKind(SymbolKind::Unknown);
        this->_symTable->pushSymbol(newSymbol);
    }
    checkSymbol = this->_symTable->getSymbolByName(labelDeclNode->getName()->get_value());
    checkSymbol->setIsDefinedByLabel(true);
    std::string parent = getParent(labelDeclNode->getName()->get_value());
    if (!parent.empty())
    {
        Symbol* parentSymbol = this->_symTable->getSymbolByName(parent);
        if (parentSymbol == nullptr)
        {
            this->_diagMngr->log(
                DiagLevel::ERROR, 0,
                "Attempted to inherit parent symbol type but parent symbol `%s` not found\n",
                parent.c_str());
        }
        checkSymbol->setSymbolKind(parentSymbol->getSymbolKind());
        checkSymbol->setIsChild(true);
    }
}
void SemanticAnalyzer::verifyTypeDecl(TypeDeclarationNode* typeDeclNode)
{
    Symbol* symbol = this->_symTable->getSymbolByName(typeDeclNode->getName()->get_value());
    if (symbol == nullptr)
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0, "Cannot set type of non existant symbol `%s`\n",
                             typeDeclNode->getName()->get_value().c_str());
    }
    SymbolKind symKind = stringToSymKind(typeDeclNode->getType()->get_value());
    if (symKind == SymbolKind::Unknown)
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0,
                             "Invalid symbol kind (%s) for a symbol type specifier\n",
                             typeDeclNode->getType()->get_value().c_str());
    }
    if (symKind == SymbolKind::Section)
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0,
                             "Cannot explicitly set symbol `%s` to a section\n",
                             symbol->getName().c_str());
    }
    if (symbol->getSymbolKind() != SymbolKind::Unset &&
        symbol->getSymbolKind() != SymbolKind::Unknown)
    {

        if (symbol->getSymbolKind() == symKind)
        {
            this->_diagMngr->log(DiagLevel::WARNING, 0,
                                 "Useless type overwrite for symbol `%s` (Overwriting "
                                 "from `%s` to the same type)\n",
                                 symbol->getName().c_str(),
                                 symKindToString(symbol->getSymbolKind()).c_str());
        }
        else if (symbol->getSymbolKind() == SymbolKind::Section)
        {
            this->_diagMngr->log(
                DiagLevel::ERROR, 0,
                "Manually overriding of types is not allowed for section. Referring to "
                "attempted overwrite of section `%s`\n",
                symbol->getName().c_str());
        }
        else
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Attempted to override previous symbol `%s` of old "
                                 "type `%s` with the new type of `%s`\n",
                                 symbol->getName().c_str(),
                                 symKindToString(symbol->getSymbolKind()).c_str(),
                                 symKindToString(symKind).c_str());
        }
    }
    symbol->setSymbolKind(symKind);
    // Set child nodes
    for (Symbol* childSymbol : this->_symTable->getSymbols())
    {
        if (childSymbol->getName().starts_with(symbol->getName()) &&
            childSymbol->getName().size() > symbol->getName().size() &&
            childSymbol->getName().at(symbol->getName().size()) == '.')
        {
            childSymbol->setSymbolKind(symKind);
        }
    }
}
void SemanticAnalyzer::verifyArgDecl(ArgumentsDeclarationNode* argDeclNode)
{
    Symbol* symbol = this->_symTable->getSymbolByName(argDeclNode->getName()->get_value());
    if (symbol == nullptr)
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0,
                             "Cannot set arguments of non existant symbol `%s`\n",
                             argDeclNode->getName()->get_value().c_str());
    }
    if (symbol->getSymbolKind() != SymbolKind::Function)
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0, "Cannot set arguments count of symbol type %lu\n",
                             symbol->getSymbolKind());
    }
    size_t argCount = std::stoul(argDeclNode->getCount()->get_value());
    if (symbol->getArgumentsCount() == argCount)
    {
        this->_diagMngr->log(DiagLevel::WARNING, 0,
                             "Senseless overwrite of symbol `%s` with previous %lu\n",
                             symbol->getName().c_str(), argCount);
    }
    else if (symbol->getArgumentsCount() != (size_t)-1)
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0,
                             "Cannot overwrite arguments count from %lu to %lu of symbol `%s`\n",
                             symbol->getArgumentsCount(), argCount, symbol->getName().c_str());
    }
    symbol->setArgumentsCount(argCount);
}
void SemanticAnalyzer::verifyDeclaration(DeclarationNode* declNode)
{
    switch (declNode->getDeclType())
    {
    case DeclarationNodeType::Global:
    {
        GlobalDeclarationNode* globalDeclNode = reinterpret_cast<GlobalDeclarationNode*>(declNode);
        this->verifyGlobalDecl(globalDeclNode);
    }
    break;
    case DeclarationNodeType::Extern:
    {
        ExternDeclarationNode* externDeclNode = reinterpret_cast<ExternDeclarationNode*>(declNode);
        this->verifyExternDecl(externDeclNode);
    }
    break;
    case DeclarationNodeType::Section:
    {
        SectionDeclarationNode* sectionDecl = reinterpret_cast<SectionDeclarationNode*>(declNode);
        this->verifySectionDecl(sectionDecl);
    }
    break;
    case DeclarationNodeType::Label:
    {
        LabelDeclarationNode* labelDeclNode = reinterpret_cast<LabelDeclarationNode*>(declNode);
        this->verifyLabelDecl(labelDeclNode);
    }
    break;
    case DeclarationNodeType::Type:
    {
        TypeDeclarationNode* typeDeclNode = reinterpret_cast<TypeDeclarationNode*>(declNode);
        this->verifyTypeDecl(typeDeclNode);
    }
    break;
    case DeclarationNodeType::Arguments:
    {
        ArgumentsDeclarationNode* argDeclNode =
            reinterpret_cast<ArgumentsDeclarationNode*>(declNode);
        this->verifyArgDecl(argDeclNode);
    }
    break;
    default:
    {
        this->_diagMngr->log(DiagLevel::ICE, 0, "Unhandled Sema declaration node with type %lu\n",
                             (size_t)declNode->getDeclType());
    }
    break;
    }
}
void SemanticAnalyzer::firstPass()
{
    for (AstNode* node : this->_ast->getNodes())
    {
        switch (node->getAstNodeType())
        {
        case AstNodeType::Declaration:
        {
            DeclarationNode* declNode = reinterpret_cast<DeclarationNode*>(node);
            if (declNode->getDeclType() != DeclarationNodeType::Type &&
                declNode->getDeclType() != DeclarationNodeType::Arguments)
            {
                this->verifyDeclaration(declNode);
            }
        }
        break;
        case AstNodeType::Expression:
        case AstNodeType::Direct:
        case AstNodeType::Instruction:
            break;
        default:
        {
            this->_diagMngr->log(DiagLevel::WARNING, 0,
                                 "Unhandled Sema node first pass with type %lu\n",
                                 (size_t)node->getAstNodeType());
        }
        break;
        }
    }
}
void SemanticAnalyzer::secondPass()
{
    for (AstNode* node : this->_ast->getNodes())
    {
        switch (node->getAstNodeType())
        {
        case AstNodeType::Declaration:
        {
            DeclarationNode* declNode = reinterpret_cast<DeclarationNode*>(node);
            if (declNode->getDeclType() == DeclarationNodeType::Type)
            {
                this->verifyDeclaration(declNode);
            }
            else if (declNode->getDeclType() == DeclarationNodeType::Arguments)
            {
                this->_toHandleNodes.push_back(node);
            }
        }
        break;
        case AstNodeType::Expression:
        case AstNodeType::Instruction:
        {
            this->_toHandleNodes.push_back(node);
        }
        break;
        case AstNodeType::Direct:
            break;
        default:
        {
            this->_diagMngr->log(DiagLevel::ICE, 0,
                                 "Unhandled Sema node second pass with type %lu\n",
                                 (size_t)node->getAstNodeType());
        }
        break;
        }
    }
}
std::pair<ExpressionNodeType, uint8_t> SemanticAnalyzer::verifyExpression(ExpressionNode* exprNode)
{
    switch (exprNode->getExprType())
    {
    case ExpressionNodeType::Register:
    {
        RegisterExpressionNode* regNode = reinterpret_cast<RegisterExpressionNode*>(exprNode);
        if (std::find(utils::registers.begin(), utils::registers.end(),
                      regNode->getRegister()->get_value()) == utils::registers.end())
        {
            this->_diagMngr->log(DiagLevel::ICE, 0, "Invalid register passed trough to Sema.\n");
        }
        uint8_t registerSize = utils::getRegisterSize(regNode->getRegister()->get_value());
        return {ExpressionNodeType::Register, registerSize};
    }
    break;
    case ExpressionNodeType::Immediate:
    {
        ImmediateExpressionNode* immNode = reinterpret_cast<ImmediateExpressionNode*>(exprNode);
        uint64_t                 value   = std::stoull(immNode->getValue()->get_value());
        uint8_t                  immSize =
            value < UINT8_MAX ? 8 : (value < UINT16_MAX ? 16 : (value < UINT32_MAX ? 32 : 64));
        return {ExpressionNodeType::Immediate, immSize};
    }
    break;
    case ExpressionNodeType::Memory:
    {
        MemoryExpressionNode* memNode = reinterpret_cast<MemoryExpressionNode*>(exprNode);
        AstNode*              base    = memNode->getBase();
        if (base->getAstNodeType() != AstNodeType::Expression)
        {
            this->_diagMngr->log(DiagLevel::ICE, 0,
                                 "Memory expression base was not an expression.\n");
        }
        ExpressionNode* baseExpr = reinterpret_cast<ExpressionNode*>(base);
        if (baseExpr->getExprType() != ExpressionNodeType::Register &&
            baseExpr->getExprType() != ExpressionNodeType::Variable)
        {
            this->_diagMngr->log(
                DiagLevel::ERROR, 0,
                "Invalid memory base argument, expected register or a symbol but got %lu.\n",
                (size_t)baseExpr->getExprType());
        }
        AstNode* index = memNode->getIndex();
        if (index != nullptr)
        {
            if (index->getAstNodeType() != AstNodeType::Expression)
            {
                this->_diagMngr->log(DiagLevel::ICE, 0,
                                     "Memory expression index was not an expression.\n");
            }
            ExpressionNode* indexExpr = reinterpret_cast<ExpressionNode*>(index);
            if (indexExpr->getExprType() != ExpressionNodeType::Register)
            {
                this->_diagMngr->log(
                    DiagLevel::ERROR, 0,
                    "Invalid memory index argument, expected register but got %lu.\n",
                    (size_t)indexExpr->getExprType());
            }
        }
        AstNode* scale = memNode->getScale();
        if (scale != nullptr)
        {
            if (scale->getAstNodeType() != AstNodeType::Expression)
            {
                this->_diagMngr->log(DiagLevel::ICE, 0,
                                     "Memory expression scale was not an expression\n");
            }
            ExpressionNode* scaleExpr = reinterpret_cast<ExpressionNode*>(scale);
            if (scaleExpr->getExprType() != ExpressionNodeType::Immediate)
            {
                this->_diagMngr->log(DiagLevel::ERROR, 0,
                                     "Invalid memory scale argument, expected an "
                                     "immediate but got %lu.\n",
                                     (size_t)scaleExpr->getExprType());
            }
            ImmediateExpressionNode* immScaleExpr =
                reinterpret_cast<ImmediateExpressionNode*>(scaleExpr);
            if ((std::stoul(immScaleExpr->getValue()->get_value()) > 8 ||
                 std::stoul(immScaleExpr->getValue()->get_value()) == 0) ||
                ((std::stoul(immScaleExpr->getValue()->get_value()) &
                  (std::stoul(immScaleExpr->getValue()->get_value()) - 1)) != 0))
            {
                this->_diagMngr->log(DiagLevel::ERROR, 0,
                                     "Invalid memory scale argument, expected a value "
                                     "of 1, 2, 4 or 8 but got %lu.\n",
                                     std::stoul(immScaleExpr->getValue()->get_value()));
            }
        }
        AstNode* displacement = memNode->getDisplacement();
        if (displacement != nullptr)
        {
            if (displacement->getAstNodeType() != AstNodeType::Expression)
            {
                this->_diagMngr->log(DiagLevel::ICE, 0,
                                     "Memory expression displacement was not an expression\n");
            }
            ExpressionNode* dispExpr = reinterpret_cast<ExpressionNode*>(displacement);
            if (dispExpr->getExprType() != ExpressionNodeType::Immediate)
            {
                this->_diagMngr->log(DiagLevel::ERROR, 0,
                                     "Invalid memory displacement argument, expected an "
                                     "immediate but got %lu. Did you mean to enable constant "
                                     "folding? (-fconst-fold)\n",
                                     (size_t)dispExpr->getExprType());
            }
            ImmediateExpressionNode* immScaleExpr =
                reinterpret_cast<ImmediateExpressionNode*>(dispExpr);
            if (std::stoul(immScaleExpr->getValue()->get_value()) > UINT32_MAX)
            {
                this->_diagMngr->log(DiagLevel::ERROR, 0,
                                     "Invalid memory displacement argument, expected a "
                                     "value less than %lu but got %lu.",
                                     UINT32_MAX, std::stoul(immScaleExpr->getValue()->get_value()));
            }
        }
        return {ExpressionNodeType::Memory, 0};
    }
    break;
    case ExpressionNodeType::Sized:
    {
        SizedExpressionNode* sizedExpr         = reinterpret_cast<SizedExpressionNode*>(exprNode);
        ExpressionNode*      sizedExprNodeExpr = sizedExpr->getExpr();
        uint8_t              sizedSize         = sizedExpr->getSize();
        std::pair<ExpressionNodeType, uint8_t> result = this->verifyExpression(sizedExprNodeExpr);
        result.second                                 = sizedSize;
        return result;
    }
    break;
    case ExpressionNodeType::Variable:
    {
        VariableExpressionNode* varExprNode = reinterpret_cast<VariableExpressionNode*>(exprNode);
        if (this->_symTable->getSymbolByName(varExprNode->getName()->get_value()) == nullptr)
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Symbol `%s` not found in current symbol table.\n",
                                 varExprNode->getName()->get_value().c_str());
        }
        return {ExpressionNodeType::Variable, 64};
    }
    break;
    case ExpressionNodeType::Binary:
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0,
                             "Binary expressions aren't allowed as instruction arguments. Did you "
                             "mean to enable constant folding? (-fconst-fold)\n");
    }
    break;
    default:
    {
        this->_diagMngr->log(DiagLevel::ICE, 0,
                             "Unhandled Sema expression node third pass with type %lu\n",
                             (size_t)exprNode->getExprType());
    }
    break;
    }
    __builtin_unreachable();
}
void SemanticAnalyzer::verifyInstruction(InstructionNode*& instNode)
{
    std::vector<std::pair<ExpressionNodeType, uint8_t>> arguments;
    for (AstNode* arg : instNode->getArgs())
    {
        if (arg->getAstNodeType() != AstNodeType::Expression)
        {
            this->_diagMngr->log(DiagLevel::ICE, 0, "Instruction argument was not an expression\n");
        }
        ExpressionNode* exprNode = reinterpret_cast<ExpressionNode*>(arg);
        arguments.push_back(this->verifyExpression(exprNode));
    }
    if (arguments.size() > 2)
    {
        this->_diagMngr->log(DiagLevel::ERROR, 0,
                             "Too many arguments specified for instruction `%s`\n",
                             instNode->getMnemonic()->get_value().c_str());
    }
    uint8_t instSize = 0;
    if (arguments.size() == 2)
    {
        std::pair<ExpressionNodeType, uint8_t> destArg = arguments.at(0);
        std::pair<ExpressionNodeType, uint8_t> srcArg  = arguments.at(1);
        if (destArg.first == ExpressionNodeType::Immediate)
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0, "Destination cannot be an immediate\n");
        }
        if (destArg.first == ExpressionNodeType::Binary)
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0, "Destination cannot be a binary operation\n");
        }
        if (destArg.second == srcArg.second && destArg.second != 0)
        {
            instSize = destArg.second;
        }
        else if (destArg.first == ExpressionNodeType::Register &&
                 !instNode->getMnemonic()->get_value().ends_with("x"))
        {
            if (srcArg.first == ExpressionNodeType::Immediate)
            {
                instSize = destArg.second;
            }
            else if (srcArg.first == ExpressionNodeType::Memory && srcArg.second == 0)
            {
                instSize = destArg.second;
            }
            else if (srcArg.second > destArg.second)
            {
                this->_diagMngr->log(DiagLevel::ERROR, 0,
                                     "Invalid instruction sizes specified, source size is greater "
                                     "than the destination size\n");
            }
            else if (srcArg.second < destArg.second)
            {
                this->_diagMngr->log(DiagLevel::ERROR, 0,
                                     "Invalid instruction sizes specified, source size is smaller "
                                     "than the destination size\n");
            }
            else if (srcArg.first == ExpressionNodeType::Memory)
            {
                instSize = destArg.second;
            }
            else
            {
                std::printf("TODO: destArg.first == ExpressionNodeType::Register\n");
                std::exit(1);
            }
        }
        else if (destArg.first == ExpressionNodeType::Register &&
                 instNode->getMnemonic()->get_value().ends_with("x"))
        {
            instSize = destArg.second;
        }
        else if (destArg.first == ExpressionNodeType::Memory && destArg.second != 0)
        {
            if (srcArg.first == ExpressionNodeType::Memory)
            {
                this->_diagMngr->log(
                    DiagLevel::ERROR, 0,
                    "Cannot have both the destination and source as memory addresses\n");
            }
            else if (srcArg.first == ExpressionNodeType::Immediate)
            {
                instSize = destArg.second;
            }
            else
            {
                std::printf(
                    "TODO: destArg.first == ExpressionNodeType::Memory && destArg.second != 0\n");
            }
        }
        else if (destArg.first == ExpressionNodeType::Memory)
        {
            if (srcArg.first == ExpressionNodeType::Memory)
            {
                this->_diagMngr->log(
                    DiagLevel::ERROR, 0,
                    "Cannot have both the destination and source as memory addresses\n");
            }
            else if (srcArg.first == ExpressionNodeType::Immediate)
            {
                instSize = srcArg.second;
            }
            else if (srcArg.first == ExpressionNodeType::Register)
            {
                instSize = srcArg.second;
            }
            else
            {
                std::printf("TODO: destArg.first == ExpressionNodeType::Memory\n");
            }
        }
        // else if (srcArg.first == ExpressionNodeType::Register)
        // {
        //     if (destArg.first == ExpressionNodeType::Immediate)
        //     {
        //         instSize = srcArg.second;
        //     }
        //     else if (destArg.second > srcArg.second)
        //     {
        //         this->_diagMngr->log(
        //             DiagLevel::ERROR, 0,
        //             "Invalid instruction sizes specified, destination size is greater "
        //             "than the source size\n");
        //     }
        //     else if (destArg.second < srcArg.second)
        //     {
        //         this->_diagMngr->log(
        //             DiagLevel::ERROR, 0,
        //             "Invalid instruction sizes specified, destination size is smaller "
        //             "than the source size\n");
        //     }
        //     else if (destArg.first == ExpressionNodeType::Memory)
        //     {
        //         instSize = srcArg.second;
        //     }
        //     else
        //     {
        //         std::printf("TODO: destArg.first == ExpressionNodeType::Register\n");
        //         std::exit(1);
        //     }
        // }
        // else if (srcArg.first == ExpressionNodeType::Memory && srcArg.second != 0)
        // {
        //     std::printf("TODO: srcArg.first == ExpressionNodeType::Memory\n");
        //     std::exit(1);
        // }
        else
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Could not figure out instruction size for instruction `%s` "
                                 "(Operands were {%lu, %hhu} and {%lu, %hhu}\n",
                                 instNode->getMnemonic()->get_value().c_str(),
                                 (size_t)destArg.first, destArg.second, (size_t)srcArg.first,
                                 srcArg.second);
        }
    }
    else if (arguments.size() == 1)
    {
        instSize = arguments.at(0).second;
    }
    else if (arguments.size() == 0)
    {
        instSize = 0;
    }
    else
    {
        this->_diagMngr->log(
            DiagLevel::ICE, 0,
            "Instruction size couldn't be found due to unhandled argument count of %llu\n",
            arguments.size());
    }
    instNode->setInstSize(instSize);
    if (utils::getBranchInstruction(instNode->getMnemonic()->get_value()))
    {
        if (arguments.size() != 1)
        {
            this->_diagMngr->log(
                DiagLevel::ERROR, 0,
                "Malformed branch instruction, expected 1 argument but got %lu arguments\n",
                arguments.size());
        }
        Symbol* arg = this->_symTable->getSymbolByName(
            reinterpret_cast<VariableExpressionNode*>(
                reinterpret_cast<ExpressionNode*>(instNode->getArgs().at(0)))
                ->getName()
                ->get_value());
        if (arguments.at(0).first != ExpressionNodeType::Register &&
            arg->getSymbolKind() != SymbolKind::Function)
        {
            this->_diagMngr->log(
                DiagLevel::ERROR, 0,
                "Malformed branch instruction, expected function argument but got %lu (%lu)\n",
                (size_t)arguments.at(0).first, (size_t)arg->getSymbolKind());
        }
    }
}
void SemanticAnalyzer::thirdPass()
{
    for (AstNode*& node : _toHandleNodes)
    {
        switch (node->getAstNodeType())
        {
        case AstNodeType::Declaration:
        {
            DeclarationNode* declNode = reinterpret_cast<DeclarationNode*>(node);
            this->verifyDeclaration(declNode);
        }
        break;
        case AstNodeType::Instruction:
        {
            InstructionNode* instNode = reinterpret_cast<InstructionNode*>(node);
            this->verifyInstruction(instNode);
        }
        break;
        default:
        {
            this->_diagMngr->log(DiagLevel::ICE, 0,
                                 "Unhandled Sema node third pass with type %lu\n",
                                 (size_t)node->getAstNodeType());
        }
        break;
        }
    }
}
void SemanticAnalyzer::verify()
{
    this->firstPass();
    this->secondPass();
    for (Symbol* sym : this->_symTable->getSymbols())
    {
        if (sym->getSymbolKind() == SymbolKind::Unset ||
            sym->getSymbolKind() == SymbolKind::Unknown)
        {
            this->_diagMngr->log(DiagLevel::ERROR, 0,
                                 "Symbol `%s` could not have its kind resolved\n",
                                 sym->getName().c_str());
        }
        if (sym->getSymbolBind() == SymbolBinding::Extern && sym->getIsDefinedByLabel())
        {
            this->_diagMngr->log(
                DiagLevel::ERROR, 0,
                "Symbol `%s` was declared external but its definition is given in the file\n",
                sym->getName().c_str());
        }
    }
    // TODO(luccie-cmd): Verify symbol table to ensure everything has a symbol kind (function,
    // object, etc)
    this->thirdPass();
}
}; // namespace assembler