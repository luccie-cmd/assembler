#include <format>
#include <opts/constantFolding.h>
#include <syntax/ast/expr.h>
#include <syntax/ast/inst.h>

namespace assembler::opts
{
ConstantFoldingPass::ConstantFoldingPass(bool fold)
{
    this->fold = fold;
    this->name = "Constant Folding";
}
ConstantFoldingPass::~ConstantFoldingPass() {}
std::optional<std::pair<DiagLevel, std::string>>
ConstantFoldingPass::foldExpr(ExpressionNode*& expr)
{
    switch (expr->getExprType())
    {
    case ExpressionNodeType::Register:
    case ExpressionNodeType::String:
    case ExpressionNodeType::Variable:
    case ExpressionNodeType::Immediate:
        break;
    case ExpressionNodeType::Sized:
    {
        ExpressionNode* exprSized = reinterpret_cast<SizedExpressionNode*>(expr)->getExpr();
        return this->foldExpr(exprSized);
    }
    break;
    case ExpressionNodeType::Binary:
    {
        BinaryExpressionNode* binExpr = reinterpret_cast<BinaryExpressionNode*>(expr);
        AstNode*              lhsNode = binExpr->getLhs();
        AstNode*              rhsNode = binExpr->getRhs();
        ExpressionNode*&      lhs     = reinterpret_cast<ExpressionNode*&>(lhsNode);
        ExpressionNode*&      rhs     = reinterpret_cast<ExpressionNode*&>(rhsNode);
        std::optional<std::pair<DiagLevel, std::string>> lhsResult = foldExpr(lhs);
        if (lhsResult.has_value())
        {
            return lhsResult;
        }
        std::optional<std::pair<DiagLevel, std::string>> rhsResult = foldExpr(rhs);
        if (rhsResult.has_value())
        {
            return rhsResult;
        }
        if (lhs->getExprType() != ExpressionNodeType::Immediate ||
            rhs->getExprType() != ExpressionNodeType::Immediate)
        {
            break;
        }
        ImmediateExpressionNode* lhsImm = reinterpret_cast<ImmediateExpressionNode*>(lhs);
        ImmediateExpressionNode* rhsImm = reinterpret_cast<ImmediateExpressionNode*>(rhs);
        uint64_t                 lhsVal = std::stoul(lhsImm->getValue()->get_value());
        uint64_t                 rhsVal = std::stoul(rhsImm->getValue()->get_value());
        if (!this->fold)
        {
            return {
                {DiagLevel::WARNING,
                 std::format("expression {} {} {} could be folded but constant folding is disabled",
                             lhsVal, binExpr->getOperation()->get_value().at(0), rhsVal)}};
        }
        switch (binExpr->getOperation()->get_value().at(0))
        {
        case '*':
        {
            expr = new ImmediateExpressionNode(
                new Token(std::to_string(lhsVal * rhsVal), TokenType::LIT_NUMBER));
        }
        break;
        case '+':
        {
            expr = new ImmediateExpressionNode(
                new Token(std::to_string(lhsVal + rhsVal), TokenType::LIT_NUMBER));
        }
        break;
        default:
        {
            return {{DiagLevel::ERROR, std::format("Invalid binary fold operator {}",
                                                   binExpr->getOperation()->get_value().at(0))}};
        }
        break;
        }
    }
    break;
    case ExpressionNodeType::Memory:
    {
        MemoryExpressionNode* memExpr  = reinterpret_cast<MemoryExpressionNode*>(expr);
        AstNode*&             nodeDisp = memExpr->getDisplacement();
        if (nodeDisp == nullptr)
        {
            break;
        }
        ExpressionNode*& exprDisp = reinterpret_cast<ExpressionNode*&>(nodeDisp);
        return foldExpr(exprDisp);
    }
    break;
    default:
    {
        return {{DiagLevel::ERROR, std::format("Unhandled optimizer expression type: {}",
                                               (size_t)expr->getExprType())}};
    }
    break;
    }
    return {};
}
std::optional<std::pair<DiagLevel, std::string>> ConstantFoldingPass::foldNode(AstNode* node)
{
    switch (node->getAstNodeType())
    {
    case AstNodeType::Instruction:
    {
        InstructionNode* instNode = reinterpret_cast<InstructionNode*>(node);
        for (AstNode*& instArg : instNode->getArgs())
        {
            ExpressionNode*& exprNode = reinterpret_cast<ExpressionNode*&>(instArg);
            std::optional<std::pair<DiagLevel, std::string>> err = this->foldExpr(exprNode);
            if (err.has_value())
            {
                return err;
            }
        }
    }
    break;
    case AstNodeType::Expression:
    {
        ExpressionNode* expr = reinterpret_cast<ExpressionNode*>(node);
        return this->foldExpr(expr);
    }
    break;
    case AstNodeType::Direct:
    case AstNodeType::Declaration:
        break;
    default:
    {
        return {{DiagLevel::ERROR,
                 std::format("Unhandled optimizer node: {}", (size_t)node->getAstNodeType())}};
    }
    break;
    }
    return {};
}
std::vector<std::pair<DiagLevel, std::string>> ConstantFoldingPass::run(std::vector<AstNode*> nodes)
{
    std::vector<std::pair<DiagLevel, std::string>> errors;
    for (AstNode* node : nodes)
    {
        std::optional<std::pair<DiagLevel, std::string>> result = foldNode(node);
        if (result.has_value())
        {
            errors.push_back(result.value());
        }
    }
    return errors;
}
std::string ConstantFoldingPass::getName()
{
    return this->name;
}
}; // namespace assembler::opts