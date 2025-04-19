#include <slir/gen/irgen.h>
#include <syntax/ast/decl.h>
#include <syntax/ast/inst.h>

namespace assembler::ir::gen
{
IrGen::IrGen(DiagManager* diagMngr, Ast* ast, SymbolTable* symTable)
{
    this->_diagMngr = diagMngr;
    this->_ast      = ast;
    this->_symTable = symTable;
}
ir::Function* IrGen::genFunction(std::string name, std::vector<AstNode*> nodes)
{
    OrderedMap<std::string, std::vector<AstNode*>> blocks;
    std::string                                    currentBlock = name;
    blocks.insert_or_assign(currentBlock, std::vector<AstNode*>({}));
    for (AstNode* node : nodes)
    {
        if (node->getAstNodeType() == AstNodeType::Declaration)
        {
            DeclarationNode*      declNode  = reinterpret_cast<DeclarationNode*>(node);
            LabelDeclarationNode* labelDecl = reinterpret_cast<LabelDeclarationNode*>(declNode);
            currentBlock                    = labelDecl->getName()->get_value();
            blocks.insert_or_assign(currentBlock, std::vector<AstNode*>({}));
            continue;
        }
        blocks.at(currentBlock).push_back(node);
    }
    for (const auto& [name, nodes] : blocks)
    {
        std::printf("- %s\n", name.c_str());
        for (AstNode* node : nodes)
        {
            if (node->getAstNodeType() == AstNodeType::Instruction)
            {
                InstructionNode* instNode = reinterpret_cast<InstructionNode*>(node);
                std::printf("  - inst: %s\n", instNode->getMnemonic()->get_value().c_str());
            }
        }
    }
    this->_diagMngr->log(DiagLevel::ICE, 0, "TODO: Emit function `%s`\n", name.c_str());
    return nullptr;
}
ir::Section* IrGen::genSection(std::string name, std::vector<AstNode*> nodes)
{
    OrderedMap<std::string, std::vector<AstNode*>> functions;
    std::string                                    currentFunction;
    OrderedMap<std::string, std::vector<AstNode*>> objects;
    std::string                                    currentObject;
    for (AstNode* node : nodes)
    {
        if (node->getAstNodeType() == AstNodeType::Declaration)
        {
            DeclarationNode* declNode = reinterpret_cast<DeclarationNode*>(node);
            if (declNode->getDeclType() != DeclarationNodeType::Label)
            {
                this->_diagMngr->log(DiagLevel::ICE, 0,
                                     "A declaration node with type %lu slipped trough\n",
                                     declNode->getDeclType());
            }
            LabelDeclarationNode* labelDecl = reinterpret_cast<LabelDeclarationNode*>(declNode);
            Symbol* sym = this->_symTable->getSymbolByName(labelDecl->getName()->get_value());
            if (sym->getSymbolKind() == SymbolKind::Function && !sym->getIsChild())
            {
                if (functions.contains(sym->getName()))
                {
                    this->_diagMngr->log(DiagLevel::ERROR, 0,
                                         "Redefinition of function body `%s`\n",
                                         sym->getName().c_str());
                }
                functions.insert_or_assign(sym->getName(), std::vector<AstNode*>({}));
                currentFunction = sym->getName();
                currentObject.clear();
                continue;
            }
            else if (sym->getSymbolKind() == SymbolKind::Object)
            {
                if (functions.contains(sym->getName()))
                {
                    this->_diagMngr->log(DiagLevel::ERROR, 0, "Redefinition of object body `%s`\n",
                                         sym->getName().c_str());
                }
                objects.insert_or_assign(sym->getName(), std::vector<AstNode*>({}));
                currentObject = sym->getName();
                currentFunction.clear();
                continue;
            }
        }
        if (!currentFunction.empty())
        {
            functions.at(currentFunction).push_back(node);
        }
        else if (!currentObject.empty())
        {
            objects.at(currentObject).push_back(node);
        }
        else
        {
            this->_diagMngr->log(
                DiagLevel::ERROR, 0,
                "Expected an object or a label before direct bytes or an instruction\n");
        }
    }
    ir::Section* section = new ir::Section(name);
    for (const auto& [name, nodes] : functions)
    {
        section->addFunction(this->genFunction(name, nodes));
    }
    for (const auto& [name, nodes] : objects)
    {
        std::printf("- %s\n", name.c_str());
        for (AstNode* node : nodes)
        {
            std::printf("  - %lu\n", node->getAstNodeType());
        }
        this->_diagMngr->log(DiagLevel::WARNING, 0, "TODO: Emit object `%s`\n", name.c_str());
    }
    // for (AstNode* node : nodes)
    // {
    //     switch (node->getAstNodeType())
    //     {
    //     case AstNodeType::Declaration:
    //     {
    //         DeclarationNode* declNode = reinterpret_cast<DeclarationNode*>(node);
    //         switch (declNode->getDeclType())
    //         {
    //         case DeclarationNodeType::Label: {
    //             LabelDeclarationNode* labelDecl =
    //             reinterpret_cast<LabelDeclarationNode*>(declNode);
    //             section->addFunction(this->genFunction(labelDecl->getName()->get_value()));
    //         } break;
    //         default:
    //         {
    //             this->_diagMngr->log(DiagLevel::ICE, 0,
    //                                  "Unhandled section generation declaration node type %lu\n",
    //                                  declNode->getDeclType());
    //         }
    //         break;
    //         }
    //     }
    //     break;
    //     default:
    //     {
    //         this->_diagMngr->log(DiagLevel::ICE, 0, "Unhandled section generation node type
    //         %lu\n",
    //                              node->getAstNodeType());
    //     }
    //     break;
    //     }
    // }
    return section;
}
ir::Module* IrGen::genModule()
{
    sections.insert_or_assign(std::string(".text"), std::vector<AstNode*>({}));
    std::string sectionName = ".text";
    // Group nodes by section
    for (AstNode* node : this->_ast->getNodes())
    {
        if (node->getAstNodeType() == AstNodeType::Declaration)
        {
            DeclarationNode* declNode = reinterpret_cast<DeclarationNode*>(node);
            if (declNode->getDeclType() == DeclarationNodeType::Section)
            {
                SectionDeclarationNode* sectionDecl =
                    reinterpret_cast<SectionDeclarationNode*>(declNode);
                if (!sections.contains(sectionDecl->getName()->get_value()))
                {
                    sections.insert_or_assign(sectionDecl->getName()->get_value(),
                                              std::vector<AstNode*>({}));
                }
                sectionName = sectionDecl->getName()->get_value();
                continue;
            }
            if (declNode->getDeclType() != DeclarationNodeType::Label)
            {
                continue;
            }
        }
        sections.at(sectionName).push_back(node);
    }
    ir::Module* _module = new ir::Module;
    for (const auto& [name, nodes] : sections)
    {
        this->genSection(name, nodes);
    }
    return _module;
}
}; // namespace assembler::ir::gen