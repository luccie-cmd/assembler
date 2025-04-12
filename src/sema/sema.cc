#include <algorithm>
#include <cstdint>
#include <sema/sema.h>
#include <syntax/ast/decl.h>
#include <syntax/ast/expr.h>
#include <syntax/ast/inst.h>

namespace utils
{
extern std::vector<std::string> registers;
uint8_t                         getRegisterSize(std::string reg);
} // namespace utils

namespace assembler
{
SymbolTable::SymbolTable()
{
    this->_symbols.clear();
}
SymbolTable::~SymbolTable()
{
    for (Symbol* sym : this->_symbols)
    {
        delete sym;
    }
    this->_symbols.clear();
}
void SymbolTable::pushSymbol(Symbol* sym)
{
    this->_symbols.push_back(sym);
}
Symbol* SymbolTable::getSymbolByName(std::string name)
{
    for (Symbol* sym : this->_symbols)
    {
        if (sym->getName() == name)
        {
            return sym;
        }
    }
    return nullptr;
}
Symbol::Symbol(SymbolBinding symbolBind, std::string name, bool isDefinedByLabel)
{
    this->symbolBind       = symbolBind;
    this->symbolKind       = SymbolKind::Unset;
    this->name             = name;
    this->isDefinedByLabel = isDefinedByLabel;
}
Symbol::~Symbol() {}
std::string Symbol::getName()
{
    return this->name;
}
SymbolKind Symbol::getSymbolKind()
{
    return this->symbolKind;
}
SymbolBinding Symbol::getSymbolBind()
{
    return this->symbolBind;
}
bool Symbol::getIsDefinedByLabel()
{
    return this->isDefinedByLabel;
}
void Symbol::setSymbolKind(SymbolKind kind)
{
    this->symbolKind = kind;
}
void Symbol::setIsDefinedByLabel(bool isDefinedByLabel)
{
    this->isDefinedByLabel = isDefinedByLabel;
}
SemanticAnalyzer::SemanticAnalyzer(Ast* inAst, DiagManager* diagManager)
{
    this->_ast      = inAst;
    this->_diagMngr = diagManager;
    this->_symTable = new SymbolTable;
}
SemanticAnalyzer::~SemanticAnalyzer()
{
    delete this->_symTable;
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
void SemanticAnalyzer::verify()
{
    for (AstNode* node : this->_ast->getNodes())
    {
        switch (node->getAstNodeType())
        {
        case AstNodeType::Declaration:
        {
            DeclarationNode* declNode = reinterpret_cast<DeclarationNode*>(node);
            switch (declNode->getDeclType())
            {
            case DeclarationNodeType::Global:
            {
                GlobalDeclarationNode* globalDeclNode =
                    reinterpret_cast<GlobalDeclarationNode*>(declNode);
                Symbol* checkSymbol =
                    this->_symTable->getSymbolByName(globalDeclNode->getName()->get_value());
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
                Symbol* symbol = new Symbol(SymbolBinding::Global,
                                            globalDeclNode->getName()->get_value(), false);
                this->_symTable->pushSymbol(symbol);
            }
            break;
            case DeclarationNodeType::Extern:
            {
                ExternDeclarationNode* externDeclNode =
                    reinterpret_cast<ExternDeclarationNode*>(declNode);
                Symbol* checkSymbol =
                    this->_symTable->getSymbolByName(externDeclNode->getName()->get_value());
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
                Symbol* symbol = new Symbol(SymbolBinding::Extern,
                                            externDeclNode->getName()->get_value(), false);
                this->_symTable->pushSymbol(symbol);
            }
            break;
            case DeclarationNodeType::Section:
            {
                SectionDeclarationNode* sectionDecl =
                    reinterpret_cast<SectionDeclarationNode*>(declNode);
                Symbol* checkSymbol =
                    this->_symTable->getSymbolByName(sectionDecl->getName()->get_value());
                if (checkSymbol != nullptr)
                {
                    if (checkSymbol->getSymbolKind() == SymbolKind::Section)
                    {
                        this->_diagMngr->log(
                            DiagLevel::ERROR, 0,
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
                Symbol* newSymbol = new Symbol(SymbolBinding::Global,
                                               sectionDecl->getName()->get_value().c_str(), false);
                newSymbol->setSymbolKind(SymbolKind::Section);
                this->_symTable->pushSymbol(newSymbol);
            }
            break;
            case DeclarationNodeType::Label:
            {
                LabelDeclarationNode* labelDeclNode =
                    reinterpret_cast<LabelDeclarationNode*>(declNode);
                Symbol* checkSymbol =
                    this->_symTable->getSymbolByName(labelDeclNode->getName()->get_value());
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
                    Symbol* newSymbol = new Symbol(
                        SymbolBinding::Global, labelDeclNode->getName()->get_value().c_str(), true);
                    newSymbol->setSymbolKind(SymbolKind::Unknown);
                    this->_symTable->pushSymbol(newSymbol);
                }
                checkSymbol =
                    this->_symTable->getSymbolByName(labelDeclNode->getName()->get_value());
                checkSymbol->setIsDefinedByLabel(true);
            }
            break;
            case DeclarationNodeType::Type:
                break;
            default:
            {
                this->_diagMngr->log(DiagLevel::ICE, 0,
                                     "Unhandled Sema declaration node first pass with type %lu\n",
                                     (size_t)declNode->getDeclType());
            }
            break;
            }
        }
        break;
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
    std::vector<AstNode*> nodesToHandle;
    for (AstNode* node : this->_ast->getNodes())
    {
        switch (node->getAstNodeType())
        {
        case AstNodeType::Declaration:
        {
            DeclarationNode* declNode = reinterpret_cast<DeclarationNode*>(node);
            switch (declNode->getDeclType())
            {
            case DeclarationNodeType::Global:
            case DeclarationNodeType::Extern:
            case DeclarationNodeType::Section:
            case DeclarationNodeType::Label:
                break;

            case DeclarationNodeType::Type:
            {
                TypeDeclarationNode* typeDeclNode =
                    reinterpret_cast<TypeDeclarationNode*>(declNode);
                Symbol* symbol =
                    this->_symTable->getSymbolByName(typeDeclNode->getName()->get_value());
                if (symbol == nullptr)
                {
                    // ERROR
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
            }
            break;
            default:
            {
                this->_diagMngr->log(DiagLevel::ICE, 0,
                                     "Unhandled Sema declaration node second pass with type %lu\n",
                                     (size_t)declNode->getDeclType());
            }
            break;
            }
        }
        break;
        case AstNodeType::Instruction:
        {
            nodesToHandle.push_back(node);
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
    // TODO(luccie-cmd): Verify symbol table to ensure everything has a symbol kind (function,
    // object, etc)
    for (AstNode* node : nodesToHandle)
    {
        switch (node->getAstNodeType())
        {
        case AstNodeType::Instruction:
        {
            // TODO: Verify argument count
            InstructionNode* instNode = reinterpret_cast<InstructionNode*>(node);
            std::vector<std::pair<ExpressionNodeType, uint8_t>> arguments;
            for (AstNode* arg : instNode->getArgs())
            {
                if (arg->getAstNodeType() != AstNodeType::Expression)
                {
                    this->_diagMngr->log(DiagLevel::ICE, 0,
                                         "Instruction argument was not an expression\n");
                }
                ExpressionNode* exprNode = reinterpret_cast<ExpressionNode*>(arg);
                switch (exprNode->getExprType())
                {
                case ExpressionNodeType::Register:
                {
                    RegisterExpressionNode* regNode =
                        reinterpret_cast<RegisterExpressionNode*>(exprNode);
                    if (std::find(utils::registers.begin(), utils::registers.end(),
                                  regNode->getRegister()->get_value()) == utils::registers.end())
                    {
                        this->_diagMngr->log(DiagLevel::ICE, 0,
                                             "Invalid register passed trough to Sema\n");
                    }
                    uint8_t registerSize =
                        utils::getRegisterSize(regNode->getRegister()->get_value());
                    arguments.push_back({ExpressionNodeType::Register, registerSize});
                }
                break;
                case ExpressionNodeType::Immediate:
                {
                    ImmediateExpressionNode* immNode =
                        reinterpret_cast<ImmediateExpressionNode*>(exprNode);
                    uint64_t value = std::stoull(immNode->getValue()->get_value());
                    std::printf("%lu\n", value);
                }
                break;
                default:
                {
                    this->_diagMngr->log(
                        DiagLevel::ICE, 0,
                        "Unhandled Sema expression node third pass with type %lu\n",
                        (size_t)exprNode->getExprType());
                }
                break;
                }
            }
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
}; // namespace assembler