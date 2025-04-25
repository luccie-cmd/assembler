#include <algorithm>
#include <any>
#include <cmath>
#include <deque>
#include <functional>
#include <set>
#include <slir/gen/irgen.h>
#include <syntax/ast/decl.h>
#include <syntax/ast/inst.h>
#include <unordered_map>

namespace utils
{
uint8_t getRegisterSize(std::string reg);
};

namespace assembler::ir::gen
{
IrGen::IrGen(DiagManager* diagMngr, Ast* ast, SymbolTable* symTable)
{
    this->_diagMngr = diagMngr;
    this->_ast      = ast;
    this->_symTable = symTable;
}
static std::vector<ir::Opcode> resultingOpcodes = {
    ir::Opcode::Const,  ir::Opcode::Copy,   ir::Opcode::Load,   ir::Opcode::Xor,
    ir::Opcode::Add,    ir::Opcode::Sub,    ir::Opcode::Imul,   ir::Opcode::Call,
    ir::Opcode::Select, ir::Opcode::IcmpEQ, ir::Opcode::IcmpLE, ir::Opcode::Phi,
};
static size_t                                           instructionCount = 0;
static std::vector<std::pair<std::string, std::string>> aliasses;
static std::string getSSAValueFromReg(std::string reg, bool stopAtFound = false)
{
    std::string name = reg;
    for (std::pair<std::string, std::string> oldNewAlias : aliasses)
    {
        if (oldNewAlias.first == name)
        {
            name = oldNewAlias.second;
            if (stopAtFound)
            {
                break;
            }
        }
    }
    return name;
}
static void addAlias(std::string oldName, std::string newName)
{
    aliasses.push_back({oldName, newName});
}
static std::string newResult()
{
    return std::string("%") + std::to_string(instructionCount++);
}
ir::Operand* IrGen::genOperand(ExpressionNode* node)
{
    switch (node->getExprType())
    {
    case ExpressionNodeType::Register:
    {
        RegisterExpressionNode* regExpr = reinterpret_cast<RegisterExpressionNode*>(node);
        std::string             ssaValue;
        if (regExpr->getRegister()->get_value().starts_with('e'))
        {
            ssaValue = getSSAValueFromReg("%r" + regExpr->getRegister()->get_value().substr(1));
        }
        else
        {
            ssaValue = getSSAValueFromReg("%" + regExpr->getRegister()->get_value());
        }
        return new ir::Operand(
            ir::OperandKind::Register,
            new ir::Type(ir::TypeKind::Integer,
                         utils::getRegisterSize(regExpr->getRegister()->get_value())),
            ssaValue);
    }
    break;
    case ExpressionNodeType::Immediate:
    {
        ImmediateExpressionNode* immExpr = reinterpret_cast<ImmediateExpressionNode*>(node);
        return new ir::Operand(ir::OperandKind::Immediate, new ir::Type(ir::TypeKind::Integer, 64),
                               std::stoul(immExpr->getValue()->get_value()));
    }
    break;
    case ExpressionNodeType::Variable:
    {
        VariableExpressionNode* varExpr = reinterpret_cast<VariableExpressionNode*>(node);
        ir::TypeKind            kind;
        std::string             name;
        if (varExpr->getName()->get_value().starts_with(this->_currentFunc->getName()))
        {
            kind = ir::TypeKind::Label;
            name = "%" + varExpr->getName()->get_value();
        }
        else
        {
            kind = ir::TypeKind::Pointer;
            name = varExpr->getName()->get_value();
        }
        ir::Operand* operand =
            new ir::Operand(ir::OperandKind::Variable, new ir::Type(kind, 64), name);
        return operand;
    }
    break;
    default:
    {
        this->_diagMngr->log(DiagLevel::ICE, 0, "TODO generate operand from expression type %lu\n",
                             node->getExprType());
    }
    break;
    }
    __builtin_unreachable();
}
std::vector<ir::Instruction*> IrGen::genExpr(ExpressionNode* node)
{
    switch (node->getExprType())
    {
    case ExpressionNodeType::Memory:
    {
        std::vector<ir::Instruction*> memInsts;
        MemoryExpressionNode*         memExpr = reinterpret_cast<MemoryExpressionNode*>(node);
        if (memExpr->getIndex())
        {
            ir::Operand* indexOperand =
                this->genOperand(reinterpret_cast<ExpressionNode*>(memExpr->getIndex()));
            ir::Operand* scaleOperand = nullptr;
            if (memExpr->getScale())
            {
                scaleOperand =
                    this->genOperand(reinterpret_cast<ExpressionNode*>(memExpr->getScale()));
            }
            else
            {
                scaleOperand = new ir::Operand(ir::OperandKind::Immediate,
                                               new ir::Type(ir::TypeKind::Integer, 64), 1);
            }
            memInsts.push_back(
                new ir::Instruction(ir::Opcode::Imul, {indexOperand, scaleOperand}, newResult()));
        }
        ir::Operand* baseOperand =
            this->genOperand(reinterpret_cast<ExpressionNode*>(memExpr->getBase()));
        if (memExpr->getIndex() != nullptr)
        {
            ir::Operand* indexSSA =
                new ir::Operand(ir::OperandKind::SSA, new ir::Type(ir::TypeKind::Integer, 64),
                                instructionCount - 1);
            memInsts.push_back(
                new ir::Instruction(ir::Opcode::Add, {baseOperand, indexSSA}, newResult()));
        }
        if (memExpr->getDisplacement() != nullptr)
        {
            ir::Operand* indexBaseSSA =
                new ir::Operand(ir::OperandKind::SSA, new ir::Type(ir::TypeKind::Integer, 64),
                                instructionCount - 1);
            memInsts.push_back(new ir::Instruction(
                ir::Opcode::Add,
                {this->genOperand(reinterpret_cast<ExpressionNode*>(memExpr->getDisplacement())),
                 indexBaseSSA},
                newResult()));
        }
        return memInsts;
    }
    break;
    default:
    {
        this->_diagMngr->log(DiagLevel::ICE, 0,
                             "TODO generate expression instructions from expression type %lu\n",
                             node->getExprType());
    }
    break;
    }
    __builtin_unreachable();
}
static std::vector<ir::Instruction*> genMov(IrGen* builder, InstructionNode* movInst)
{
    ExpressionNode*           destArg = reinterpret_cast<ExpressionNode*>(movInst->getArgs().at(0));
    ExpressionNode*           srcArg  = reinterpret_cast<ExpressionNode*>(movInst->getArgs().at(1));
    ir::Opcode                opcode;
    std::vector<ir::Operand*> operands;
    if (destArg->getExprType() == ExpressionNodeType::Register &&
        srcArg->getExprType() == ExpressionNodeType::Immediate)
    {
        operands.push_back(builder->genOperand(srcArg));
        opcode                          = ir::Opcode::Const;
        std::string             result  = newResult();
        RegisterExpressionNode* regExpr = reinterpret_cast<RegisterExpressionNode*>(destArg);
        addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()), result);
        return {new ir::Instruction(opcode, operands, result)};
    }
    else if (srcArg->getExprType() == ExpressionNodeType::Register &&
             destArg->getExprType() == ExpressionNodeType::Register)
    {
        std::string result = newResult();
        opcode             = ir::Opcode::Copy;
        operands.push_back(builder->genOperand(srcArg));
        RegisterExpressionNode* regExpr = reinterpret_cast<RegisterExpressionNode*>(destArg);
        addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()), result);
        return {new ir::Instruction(opcode, operands, result)};
    }
    else if (destArg->getExprType() == ExpressionNodeType::Sized &&
             srcArg->getExprType() == ExpressionNodeType::Immediate)
    {
        SizedExpressionNode* sizedExpr = reinterpret_cast<SizedExpressionNode*>(destArg);
        if (sizedExpr->getExpr()->getExprType() != ExpressionNodeType::Memory)
        {
            std::printf("TODO: Handle non first sized arguments of type %lu\n",
                        (size_t)sizedExpr->getExpr()->getExprType());
            std::exit(1);
        }
        std::vector<ir::Instruction*> memInsts = builder->genExpr(sizedExpr->getExpr());
        std::any                      destInstCount;
        if (memInsts.empty() || memInsts.size() == 0)
        {
            MemoryExpressionNode* memExpr =
                reinterpret_cast<MemoryExpressionNode*>(sizedExpr->getExpr());
            ExpressionNode* baseExpr = reinterpret_cast<ExpressionNode*>(memExpr->getBase());
            if (baseExpr->getExprType() == ExpressionNodeType::Register)
            {
                RegisterExpressionNode* regExpr =
                    reinterpret_cast<RegisterExpressionNode*>(baseExpr);
                destInstCount = getSSAValueFromReg("%" + regExpr->getRegister()->get_value());
            }
            else
            {
                std::printf("TODO: generate mov sized, imm mov with non register base\n");
                std::exit(1);
            }
        }
        else
        {
            destInstCount = instructionCount - 1;
        }
        ir::Operand* storeSSA;
        if (destInstCount.type() == typeid(size_t))
        {
            storeSSA = new ir::Operand(ir::OperandKind::SSA,
                                       new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                       std::any_cast<size_t>(destInstCount));
        }
        else
        {
            std::string destInstCountString = std::any_cast<std::string>(destInstCount);
            storeSSA                        = new ir::Operand(ir::OperandKind::Register,
                                                              new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                                              destInstCountString);
        }
        ir::Operand* storeValueOperand =
            builder->genOperand(reinterpret_cast<ExpressionNode*>(srcArg));
        ir::Instruction* storeInst =
            new ir::Instruction(ir::Opcode::Store, {storeSSA, storeValueOperand});
        memInsts.push_back(storeInst);
        return memInsts;
    }
    else if (destArg->getExprType() == ExpressionNodeType::Sized &&
             srcArg->getExprType() == ExpressionNodeType::Register)
    {
        SizedExpressionNode* sizedExpr = reinterpret_cast<SizedExpressionNode*>(destArg);
        if (sizedExpr->getExpr()->getExprType() != ExpressionNodeType::Memory)
        {
            std::printf("TODO: Handle non first sized arguments of type %lu\n",
                        (size_t)sizedExpr->getExpr()->getExprType());
            std::exit(1);
        }
        std::vector<ir::Instruction*> memInsts = builder->genExpr(sizedExpr->getExpr());
        std::any                      destInstCount;
        if (memInsts.empty() || memInsts.size() == 0)
        {
            MemoryExpressionNode* memExpr =
                reinterpret_cast<MemoryExpressionNode*>(sizedExpr->getExpr());
            ExpressionNode* baseExpr = reinterpret_cast<ExpressionNode*>(memExpr->getBase());
            if (baseExpr->getExprType() == ExpressionNodeType::Register)
            {
                RegisterExpressionNode* regExpr =
                    reinterpret_cast<RegisterExpressionNode*>(baseExpr);
                destInstCount = getSSAValueFromReg("%" + regExpr->getRegister()->get_value());
            }
            else
            {
                std::printf("TODO: generate mov sized, reg mov with non register base\n");
                std::exit(1);
            }
        }
        else
        {
            destInstCount = instructionCount - 1;
        }
        ir::Operand* storeSSA;
        if (destInstCount.type() == typeid(size_t))
        {
            storeSSA = new ir::Operand(ir::OperandKind::SSA,
                                       new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                       std::any_cast<size_t>(destInstCount));
        }
        else
        {
            std::string destInstCountString = std::any_cast<std::string>(destInstCount);
            storeSSA                        = new ir::Operand(ir::OperandKind::Register,
                                                              new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                                              destInstCountString);
        }
        ir::Operand* storeValueOperand =
            builder->genOperand(reinterpret_cast<ExpressionNode*>(srcArg));
        ir::Instruction* storeInst =
            new ir::Instruction(ir::Opcode::Store, {storeSSA, storeValueOperand});
        memInsts.push_back(storeInst);
        return memInsts;
    }
    else if (destArg->getExprType() == ExpressionNodeType::Memory &&
             srcArg->getExprType() == ExpressionNodeType::Register)
    {
        std::vector<ir::Instruction*> memInsts =
            builder->genExpr(reinterpret_cast<ExpressionNode*>(destArg));
        std::any destInstCount;
        if (memInsts.empty() || memInsts.size() == 0)
        {
            MemoryExpressionNode* memExpr =
                reinterpret_cast<MemoryExpressionNode*>(reinterpret_cast<ExpressionNode*>(destArg));
            ExpressionNode* baseExpr = reinterpret_cast<ExpressionNode*>(memExpr->getBase());
            if (baseExpr->getExprType() == ExpressionNodeType::Register)
            {
                RegisterExpressionNode* regExpr =
                    reinterpret_cast<RegisterExpressionNode*>(baseExpr);
                destInstCount = getSSAValueFromReg("%" + regExpr->getRegister()->get_value());
            }
            else
            {
                std::printf("TODO: generate mov mem, reg mov with non register base\n");
                std::exit(1);
            }
        }
        else
        {
            destInstCount = instructionCount - 1;
        }
        ir::Operand* storeSSA;
        if (destInstCount.type() == typeid(size_t))
        {
            storeSSA = new ir::Operand(ir::OperandKind::SSA,
                                       new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                       std::any_cast<size_t>(destInstCount));
        }
        else
        {
            std::string destInstCountString = std::any_cast<std::string>(destInstCount);
            storeSSA                        = new ir::Operand(ir::OperandKind::Register,
                                                              new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                                              destInstCountString);
        }
        ir::Operand* storeValueOperand =
            builder->genOperand(reinterpret_cast<ExpressionNode*>(srcArg));
        ir::Instruction* storeInst =
            new ir::Instruction(ir::Opcode::Store, {storeSSA, storeValueOperand});
        memInsts.push_back(storeInst);
        return memInsts;
    }
    else if (destArg->getExprType() == ExpressionNodeType::Register &&
             srcArg->getExprType() == ExpressionNodeType::Sized)
    {
        SizedExpressionNode* sizedExpr = reinterpret_cast<SizedExpressionNode*>(srcArg);
        if (sizedExpr->getExpr()->getExprType() != ExpressionNodeType::Memory)
        {
            std::printf("TODO: Handle non first sized arguments of type %lu\n",
                        (size_t)sizedExpr->getExpr()->getExprType());
            std::exit(1);
        }
        std::vector<ir::Instruction*> memInsts = builder->genExpr(sizedExpr->getExpr());
        std::any                      destInstCount;
        if (memInsts.empty() || memInsts.size() == 0)
        {
            MemoryExpressionNode* memExpr = reinterpret_cast<MemoryExpressionNode*>(
                reinterpret_cast<ExpressionNode*>(sizedExpr->getExpr()));
            ExpressionNode* baseExpr = reinterpret_cast<ExpressionNode*>(memExpr->getBase());
            if (baseExpr->getExprType() == ExpressionNodeType::Register)
            {
                RegisterExpressionNode* regExpr =
                    reinterpret_cast<RegisterExpressionNode*>(baseExpr);
                destInstCount = getSSAValueFromReg("%" + regExpr->getRegister()->get_value());
            }
            else
            {
                std::printf("TODO: generate mov reg, sized mov with non register base\n");
                std::exit(1);
            }
        }
        else
        {
            destInstCount = instructionCount - 1;
        }
        ir::Operand* loadSSA;
        if (destInstCount.type() == typeid(size_t))
        {
            loadSSA = new ir::Operand(ir::OperandKind::SSA,
                                      new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                      std::any_cast<size_t>(destInstCount));
        }
        else
        {
            std::string destInstCountString = std::any_cast<std::string>(destInstCount);
            loadSSA                         = new ir::Operand(ir::OperandKind::Register,
                                                              new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                                              destInstCountString);
        }
        std::string      result   = newResult();
        ir::Instruction* loadInst = new ir::Instruction(ir::Opcode::Load, {loadSSA}, result);
        memInsts.push_back(loadInst);
        RegisterExpressionNode* regExpr =
            reinterpret_cast<RegisterExpressionNode*>(reinterpret_cast<ExpressionNode*>(destArg));
        addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()), result);
        return memInsts;
    }
    else if (destArg->getExprType() == ExpressionNodeType::Register &&
             srcArg->getExprType() == ExpressionNodeType::Memory)
    {
        std::vector<ir::Instruction*> memInsts = builder->genExpr(srcArg);
        std::any                      destInstCount;
        if (memInsts.empty() || memInsts.size() == 0)
        {
            MemoryExpressionNode* memExpr =
                reinterpret_cast<MemoryExpressionNode*>(reinterpret_cast<ExpressionNode*>(srcArg));
            ExpressionNode* baseExpr = reinterpret_cast<ExpressionNode*>(memExpr->getBase());
            if (baseExpr->getExprType() == ExpressionNodeType::Register)
            {
                RegisterExpressionNode* regExpr =
                    reinterpret_cast<RegisterExpressionNode*>(baseExpr);
                destInstCount =
                    std::string(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()));
            }
            else if (baseExpr->getExprType() == ExpressionNodeType::Variable)
            {
                VariableExpressionNode* varExpr =
                    reinterpret_cast<VariableExpressionNode*>(baseExpr);
                destInstCount = std::string("@" + varExpr->getName()->get_value());
            }
            else
            {
                std::printf("TODO: generate mov reg, mem mov with non register base\n");
                std::exit(1);
            }
        }
        else
        {
            destInstCount = instructionCount - 1;
        }
        ir::Operand* loadSSA;
        if (destInstCount.type() == typeid(size_t))
        {
            loadSSA = new ir::Operand(ir::OperandKind::SSA,
                                      new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                      std::any_cast<size_t>(destInstCount));
        }
        else
        {
            loadSSA = new ir::Operand(ir::OperandKind::Register,
                                      new ir::Type(ir::TypeKind::Integer, movInst->getInstSize()),
                                      std::any_cast<std::string>(destInstCount));
        }
        std::string      result   = newResult();
        ir::Instruction* loadInst = new ir::Instruction(ir::Opcode::Load, {loadSSA}, result);
        memInsts.push_back(loadInst);
        RegisterExpressionNode* regExpr =
            reinterpret_cast<RegisterExpressionNode*>(reinterpret_cast<ExpressionNode*>(destArg));
        addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()), result);
        return memInsts;
    }
    else if (destArg->getExprType() == ExpressionNodeType::Register &&
             srcArg->getExprType() == ExpressionNodeType::Variable)
    {
        std::string      result    = newResult();
        ir::Instruction* constInst = new ir::Instruction(
            ir::Opcode::Const,
            {new ir::Operand(
                ir::OperandKind::Variable, new ir::Type(ir::TypeKind::Pointer, 64),
                reinterpret_cast<VariableExpressionNode*>(reinterpret_cast<ExpressionNode*>(srcArg))
                    ->getName()
                    ->get_value())},
            result);
        RegisterExpressionNode* regExpr =
            reinterpret_cast<RegisterExpressionNode*>(reinterpret_cast<ExpressionNode*>(destArg));
        addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()), result);
        return {constInst};
    }
    else
    {
        std::printf("TODO: MOV instruction with expr %lu and %lu\n", (size_t)destArg->getExprType(),
                    (size_t)srcArg->getExprType());
        std::exit(1);
    }
    __builtin_unreachable();
}
static std::vector<ir::Instruction*> genXor(IrGen* builder, InstructionNode* xorInst)
{
    (void)builder;
    ExpressionNode*           destArg = reinterpret_cast<ExpressionNode*>(xorInst->getArgs().at(0));
    ExpressionNode*           srcArg  = reinterpret_cast<ExpressionNode*>(xorInst->getArgs().at(1));
    ir::Opcode                opcode;
    std::vector<ir::Operand*> operands;
    if (destArg->getExprType() == ExpressionNodeType::Register &&
        srcArg->getExprType() == ExpressionNodeType::Register)
    {
        RegisterExpressionNode* destReg = reinterpret_cast<RegisterExpressionNode*>(destArg);
        RegisterExpressionNode* srcReg  = reinterpret_cast<RegisterExpressionNode*>(srcArg);
        if (destReg->getRegister()->get_value() == srcReg->getRegister()->get_value())
        {
            operands.push_back(new ir::Operand(ir::OperandKind::Immediate,
                                               new ir::Type(ir::TypeKind::Integer, 64), 0));
            opcode                          = ir::Opcode::Const;
            std::string             result  = newResult();
            RegisterExpressionNode* regExpr = reinterpret_cast<RegisterExpressionNode*>(destArg);
            addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()), result);
            return {new ir::Instruction(opcode, operands, result)};
        }
        else
        {
            std::printf("TODO: XOR between regs %s and %s\n",
                        destReg->getRegister()->get_value().c_str(),
                        srcReg->getRegister()->get_value().c_str());
            std::exit(1);
        }
    }
    else
    {
        std::printf("TODO: XOR instruction with expr %lu and %lu\n", (size_t)destArg->getExprType(),
                    (size_t)srcArg->getExprType());
        std::exit(1);
    }
    __builtin_unreachable();
}
static std::vector<ir::Instruction*> genAdd(IrGen* builder, InstructionNode* addNode)
{
    if (addNode->getInstSize() != 64)
    {
        std::printf("TODO: Generate non 64 bit adds\n");
        std::exit(1);
    }
    std::vector<ir::Instruction*> retInsts = {};
    ExpressionNode* destNode = reinterpret_cast<ExpressionNode*>(addNode->getArgs().at(0));
    ExpressionNode* srcNode  = reinterpret_cast<ExpressionNode*>(addNode->getArgs().at(1));
    if (destNode->getExprType() == ExpressionNodeType::Memory)
    {
        std::printf("TODO: Generate memory destination adds\n");
        std::exit(1);
    }
    else if (destNode->getExprType() == ExpressionNodeType::Memory)
    {
        std::printf("TODO: Generate memory source adds\n");
        std::exit(1);
    }
    else if (destNode->getExprType() != ExpressionNodeType::Memory &&
             srcNode->getExprType() != ExpressionNodeType::Memory)
    {
        std::string             result      = newResult();
        ir::Operand*            srcOperand  = builder->genOperand(srcNode);
        RegisterExpressionNode* regExpr     = reinterpret_cast<RegisterExpressionNode*>(destNode);
        ir::Operand*            destOperand = builder->genOperand(destNode);
        addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()).c_str(),
                 result.c_str());
        retInsts.push_back(new ir::Instruction(ir::Opcode::Add, {destOperand, srcOperand}, result));
    }
    else
    {
        __builtin_unreachable();
    }
    return retInsts;
}
static std::vector<ir::Instruction*> genShl(IrGen* builder, InstructionNode* shlNode)
{
    std::vector<ir::Instruction*> retInsts = {};
    ExpressionNode* destNode = reinterpret_cast<ExpressionNode*>(shlNode->getArgs().at(0));
    ExpressionNode* srcNode  = reinterpret_cast<ExpressionNode*>(shlNode->getArgs().at(1));
    if (destNode->getExprType() == ExpressionNodeType::Memory)
    {
        std::printf("TODO: Generate memory destination shift lefts\n");
        std::exit(1);
    }
    else if (destNode->getExprType() == ExpressionNodeType::Memory)
    {
        std::printf("TODO: Generate memory source shift lefts\n");
        std::exit(1);
    }
    else if (destNode->getExprType() == ExpressionNodeType::Register &&
             srcNode->getExprType() == ExpressionNodeType::Immediate)
    {
        std::string              result  = newResult();
        ImmediateExpressionNode* srcImm  = reinterpret_cast<ImmediateExpressionNode*>(srcNode);
        ImmediateExpressionNode* immExpr = new ImmediateExpressionNode(
            new Token(std::to_string(std::powl(2.0, std::stof(srcImm->getValue()->get_value()))),
                      TokenType::LIT_NUMBER));
        ir::Operand*            srcOperand  = builder->genOperand(immExpr);
        RegisterExpressionNode* regExpr     = reinterpret_cast<RegisterExpressionNode*>(destNode);
        ir::Operand*            destOperand = builder->genOperand(destNode);
        addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()).c_str(),
                 result.c_str());
        retInsts.push_back(
            new ir::Instruction(ir::Opcode::Imul, {destOperand, srcOperand}, result));
    }
    else
    {
        std::printf("TODO: Generate %lu source %lu destination shift lefts\n",
                    (size_t)srcNode->getExprType(), (size_t)destNode->getExprType());
        std::exit(1);
    }
    return retInsts;
}
static std::vector<ir::Instruction*> genInc(IrGen* builder, InstructionNode* incNode)
{
    std::vector<ir::Instruction*> retInsts = {};
    ExpressionNode* destNode = reinterpret_cast<ExpressionNode*>(incNode->getArgs().at(0));
    if (destNode->getExprType() == ExpressionNodeType::Memory)
    {
        retInsts = builder->genExpr(destNode);
    }
    else if (destNode->getExprType() == ExpressionNodeType::Register)
    {
        RegisterExpressionNode* regExpr = reinterpret_cast<RegisterExpressionNode*>(destNode);
        std::string             result  = newResult();
        aliasses.push_back({regExpr->getRegister()->get_value(), result});
        ImmediateExpressionNode* immExpr =
            new ImmediateExpressionNode(new Token(std::to_string(1), TokenType::LIT_NUMBER));
        retInsts.push_back(new ir::Instruction(
            ir::Opcode::Add, {builder->genOperand(destNode), builder->genOperand(immExpr)},
            result));
    }
    else
    {
        std::printf("ERROR: Can only execute `inc` on memory or register values");
        std::exit(1);
    }
    return retInsts;
}
static std::vector<const char*> nToArgMapping = {
    "rdi", "rsi", "rdx", "rcx", "r8", "r9",
};
static const char* nToArg(size_t i)
{
    if (i < nToArgMapping.size())
    {
        return nToArgMapping.at(i);
    }
    return "stack";
}
static std::vector<ir::Instruction*> genCall(IrGen* builder, InstructionNode* callNode)
{
    (void)builder;
    ExpressionNode* destArg = reinterpret_cast<ExpressionNode*>(callNode->getArgs().at(0));
    if (destArg->getExprType() != ExpressionNodeType::Variable)
    {
        std::printf("TODO: Call to non variable %lu\n", (size_t)destArg->getExprType());
        std::exit(1);
    }
    VariableExpressionNode*   varExpr   = reinterpret_cast<VariableExpressionNode*>(destArg);
    std::string               result    = newResult();
    std::vector<ir::Operand*> operands  = {new ir::Operand(ir::OperandKind::Variable,
                                                           new ir::Type(ir::TypeKind::Label, 64),
                                                           "@" + varExpr->getName()->get_value())};
    Symbol*                   calledSym = builder->getSymbolByName(varExpr->getName()->get_value());
    for (size_t i = 0; i < calledSym->getArgumentsCount(); ++i)
    {
        RegisterExpressionNode* regExpr =
            new RegisterExpressionNode(new Token(std::string(nToArg(i)), TokenType::IDENTIFIER));
        operands.push_back(builder->genOperand(regExpr));
    }
    ir::Instruction* inst = new ir::Instruction(ir::Opcode::Call, operands, result);
    addAlias(getSSAValueFromReg("%rax"), result);
    return {inst};
}
static std::vector<ir::Instruction*> genJmp(IrGen* builder, InstructionNode* callNode)
{
    (void)builder;
    ExpressionNode* destArg = reinterpret_cast<ExpressionNode*>(callNode->getArgs().at(0));
    if (destArg->getExprType() != ExpressionNodeType::Variable)
    {
        std::printf("TODO: Jump to non variable %lu\n", (size_t)destArg->getExprType());
        std::exit(1);
    }
    VariableExpressionNode* varExpr          = reinterpret_cast<VariableExpressionNode*>(destArg);
    std::string             currentBlockName = builder->getCurrentBlock()->getName();
    std::string             targetBlockName  = varExpr->getName()->get_value();
    ir::Instruction*        inst             = new ir::Instruction(
        ir::Opcode::Branch,
        {new ir::Operand(ir::OperandKind::Variable, new ir::Type(ir::TypeKind::Label, 64),
                                            "%" + varExpr->getName()->get_value())});
    return {inst};
}
static std::vector<ir::Instruction*> genRet(IrGen* builder, InstructionNode* retNode)
{
    (void)retNode;
    ir::Instruction* inst =
        new ir::Instruction(ir::Opcode::Ret, {builder->genOperand(new VariableExpressionNode(
                                                 new Token("%TODO", TokenType::IDENTIFIER)))});
    return {inst};
}
static ir::Operand*                  leftCmp  = nullptr;
static ir::Operand*                  rightCmp = nullptr;
static std::string                   resultingCmp;
static std::vector<ir::Instruction*> genCmp(IrGen* builder, InstructionNode* cmpNode)
{
    leftCmp  = builder->genOperand(reinterpret_cast<ExpressionNode*>(cmpNode->getArgs().at(0)));
    rightCmp = builder->genOperand(reinterpret_cast<ExpressionNode*>(cmpNode->getArgs().at(1)));
    return {};
}
static std::vector<ir::Instruction*> genConditional(ir::Opcode condOpcode)
{
    if (leftCmp == nullptr || rightCmp == nullptr)
    {
        std::printf("ERROR: Attempted a conditional move that relies on unset conditionals\n");
        std::exit(1);
    }
    resultingCmp = newResult();
    return {new ir::Instruction(condOpcode, {leftCmp, rightCmp}, resultingCmp)};
}
static std::vector<ir::Instruction*> genCmove(IrGen* builder, InstructionNode* cmoveNode)
{
    ExpressionNode* destExpr = reinterpret_cast<ExpressionNode*>(cmoveNode->getArgs().at(0));
    ExpressionNode* srcExpr  = reinterpret_cast<ExpressionNode*>(cmoveNode->getArgs().at(1));
    if (destExpr->getExprType() != ExpressionNodeType::Register ||
        srcExpr->getExprType() != ExpressionNodeType::Register)
    {
        std::printf("ERROR: Invalid cmove operands. Expected 2 registers\n");
        std::exit(1);
    }
    std::vector<ir::Instruction*> retInsts = genConditional(ir::Opcode::IcmpEQ);
    std::string                   result   = newResult();
    ir::Instruction*              selectInst =
        new ir::Instruction(ir::Opcode::Select,
                            {builder->genCmpOperand(resultingCmp), builder->genOperand(srcExpr),
                             builder->genOperand(destExpr)},
                            result);
    RegisterExpressionNode* regExpr = reinterpret_cast<RegisterExpressionNode*>(destExpr);
    addAlias(getSSAValueFromReg("%" + regExpr->getRegister()->get_value()).c_str(), result.c_str());
    retInsts.push_back(selectInst);
    return retInsts;
}
static std::vector<ir::Instruction*> genJe(IrGen* builder, InstructionNode* jeNode)
{
    ExpressionNode* destExpr = reinterpret_cast<ExpressionNode*>(jeNode->getArgs().at(0));
    if (destExpr->getExprType() != ExpressionNodeType::Variable)
    {
        std::printf("ERROR: Invalid je operands. Expected label\n");
        std::exit(1);
    }
    std::vector<ir::Instruction*> retInsts          = genConditional(ir::Opcode::IcmpEQ);
    static int                    falseLabelCounter = 0;
    std::string falseLabel = builder->getCurrentFunction()->getName() + ".__false_temp" +
                             std::to_string(falseLabelCounter++);
    ir::Instruction* brInst = new ir::Instruction(
        ir::Opcode::Branch, {builder->genCmpOperand(resultingCmp), builder->genOperand(destExpr),
                             builder->genOperand(new VariableExpressionNode(
                                 new Token(falseLabel, TokenType::IDENTIFIER)))});
    retInsts.push_back(brInst);
    builder->setTempBlockName(falseLabel);
    return retInsts;
}
static std::vector<std::string> unIROpcodes = {"push", "pop"};
using GenFunc = std::function<std::vector<ir::Instruction*>(IrGen*, InstructionNode*)>;
static std::unordered_map<std::string, GenFunc> generatorMap = {
    {"mov", genMov},     {"xor", genXor}, {"call", genCall}, {"jmp", genJmp},
    {"ret", genRet},     {"add", genAdd}, {"shl", genShl},   {"cmp", genCmp},
    {"cmove", genCmove}, {"je", genJe},   {"inc", genInc}};
std::vector<ir::Instruction*> IrGen::genInstructions(AstNode* node)
{
    if (node->getAstNodeType() != AstNodeType::Instruction)
    {
        this->_diagMngr->log(DiagLevel::ICE, 0,
                             "Generate instruction for non instruction node %lu\n",
                             node->getAstNodeType());
    }
    InstructionNode* instNode = reinterpret_cast<InstructionNode*>(node);
    if (std::find(unIROpcodes.begin(), unIROpcodes.end(), instNode->getMnemonic()->get_value()) !=
        unIROpcodes.end())
    {
        return {};
    }
    auto it = generatorMap.find(instNode->getMnemonic()->get_value());
    if (it != generatorMap.end())
    {
        return it->second(this, instNode);
    }
    this->_diagMngr->log(DiagLevel::ERROR, 0, "TODO: generate instruction `%s`\n",
                         instNode->getMnemonic()->get_value().c_str());
    __builtin_unreachable();
}
ir::Operand* IrGen::genCmpOperand(std::string ssaNode)
{
    return new ir::Operand(ir::OperandKind::SSA, new ir::Type(ir::TypeKind::Integer, 1),
                           std::stoul(ssaNode.substr(1)));
}
static std::vector<ir::Opcode> terminators = {ir::Opcode::Branch, ir::Opcode::Ret};
static bool                    blockHasTerminator(ir::Block* block)
{
    ir::Instruction* last = nullptr;
    for (ir::Instruction* inst : block->getInstructions())
    {
        last = inst;
        if (std::find(terminators.begin(), terminators.end(), last->getOpcode()) !=
                terminators.end() &&
            last != block->getInstructions().back())
        {
            std::printf("ICE: A branch (jmp, je, jne, jl, jle, jg, jge) or ret was placed in the "
                        "middle of a block\n");
            std::exit(1);
        }
    }
    if (last == nullptr)
    {
        return false;
    }
    return std::find(terminators.begin(), terminators.end(), last->getOpcode()) !=
           terminators.end();
}
static void blockInsertTerminator(ir::Block* block, std::string nextBlockName)
{
    if (!blockHasTerminator(block))
    {
        ir::Instruction* inst = new ir::Instruction(
            ir::Opcode::Branch,
            {new ir::Operand(ir::OperandKind::Variable, new ir::Type(ir::TypeKind::Label, 64),
                             "%" + nextBlockName)});
        block->addInstruction(inst);
    }
}
ir::Block* IrGen::genBlock(std::string name, std::vector<AstNode*> nodes)
{
    aliasses.clear();
    ir::Block* block    = new ir::Block(name);
    this->_currentBlock = block;
    for (AstNode* node : nodes)
    {
        std::vector<ir::Instruction*> insts = this->genInstructions(node);
        for (ir::Instruction* inst : insts)
        {
            block->addInstruction(inst);
        }
    }
    block->addAliasses(aliasses);
    return block;
}
ir::Function* IrGen::genFunction(std::string name, std::vector<AstNode*> nodes)
{
    instructionCount = 0;
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
    Symbol*             sym     = this->_symTable->getSymbolByName(name);
    SymbolBinding       symBind = sym->getSymbolBind();
    ir::FunctionBinding funcBind;
    if (symBind == SymbolBinding::Extern)
    {
        funcBind = ir::FunctionBinding::Define;
    }
    else if (symBind == SymbolBinding::Local)
    {
        funcBind = ir::FunctionBinding::DeclareInternal;
    }
    else if (symBind == SymbolBinding::Global)
    {
        funcBind = ir::FunctionBinding::DeclareExternal;
    }
    else
    {
        this->_diagMngr->log(DiagLevel::ICE, 0,
                             "Symbol `%s` binding is invalid and greater than allowed\n",
                             name.c_str());
        __builtin_unreachable();
    }
    ir::Function* func = new ir::Function(name, sym->getArgumentsCount(), funcBind);
    this->_currentFunc = func;
    if (symBind != SymbolBinding::Extern)
    {
        for (size_t i = 0; i < blocks.size(); ++i)
        {
            std::string           _name  = blocks.at(i).first;
            std::vector<AstNode*> _nodes = blocks.at(i).second;
            ir::Block*            block  = this->genBlock(_name, _nodes);
            if (blocks.size() - 1 == i && !blockHasTerminator(block))
            {
                this->_diagMngr->log(
                    DiagLevel::ERROR, 0,
                    "Expected a terminator instruction as last instruction of label `%s`\n",
                    sym->getName().c_str());
            }
            if (!this->_tempBlockName.empty())
            {
                ir::Block* newBlock  = new ir::Block(this->_tempBlockName);
                ir::Block* predBlock = new ir::Block(_name);
                this->_tempBlockName.clear();
                bool             brFound             = false;
                size_t           instCountAtNewBlock = 0;
                ir::Instruction* lastResultInst      = nullptr;
                for (ir::Instruction* inst : block->getInstructions())
                {
                    if (brFound)
                    {
                        newBlock->addInstruction(inst);
                    }
                    else
                    {
                        predBlock->addInstruction(inst);
                    }
                    if (inst->getOpcode() == ir::Opcode::Branch && !brFound)
                    {
                        if (lastResultInst)
                        {
                            instCountAtNewBlock = std::stoul(lastResultInst->getResult().substr(1));
                        }
                        else
                        {
                            instCountAtNewBlock = 0;
                        }
                        brFound = true;
                    }
                    if (std::find(resultingOpcodes.begin(), resultingOpcodes.end(),
                                  inst->getOpcode()) != resultingOpcodes.end())
                    {
                        lastResultInst = inst;
                    }
                }
                std::vector<std::pair<std::string, std::string>> predAliasses;
                std::vector<std::pair<std::string, std::string>> newAliasses;
                for (size_t j = 0; j < block->getAliasses().size(); ++j)
                {
                    if (j >= instCountAtNewBlock)
                    {
                        newAliasses.push_back(block->getAliasses().at(j));
                    }
                    else
                    {
                        predAliasses.push_back(block->getAliasses().at(j));
                    }
                }
                predBlock->addAliasses(predAliasses);
                newBlock->addAliasses(newAliasses);
                if (blocks.size() - 1 != i)
                {
                    blockInsertTerminator(newBlock, blocks.at(i + 1).first);
                }
                if (!blockHasTerminator(predBlock))
                {
                    std::printf("ICE: predBlock doesn't have a terminator\n");
                    std::exit(1);
                }

                func->addBlock(predBlock);
                func->addBlock(newBlock);
            }
            else
            {
                if (blocks.size() - 1 != i)
                {
                    blockInsertTerminator(block, blocks.at(i + 1).first);
                }
                func->addBlock(block);
            }
        }
        // Connect nodes
        size_t foundRets = 0;
        for (ir::Block* block : func->getBlocks())
        {
            ir::Instruction* last = block->getInstructions().back();
            if (last->getOpcode() == ir::Opcode::Ret)
            {
                foundRets++;
                if (foundRets - 1 != 0 || block != func->getBlocks().back())
                {
                    block->removeLastInst();
                    block->addInstruction(new ir::Instruction(
                        ir::Opcode::Branch,
                        {this->genOperand(new VariableExpressionNode(new Token(
                            func->getName() + ".UnifiedReturn", TokenType::IDENTIFIER)))}));
                    last = block->getInstructions().back();
                }
            }
            if (last->getOpcode() == ir::Opcode::Branch)
            {
                std::vector<std::string> successors;
                if (last->getOperands().size() == 3)
                {
                    successors.push_back(last->getOperands().at(1)->getName());
                    successors.push_back(last->getOperands().at(2)->getName());
                }
                else
                {
                    successors.push_back(last->getOperands().at(0)->getName());
                }
                for (std::string succ : successors)
                {
                    block->addSuccessors(succ.substr(1));
                    func->getBlockByName(succ.substr(1))->addPredecessors(block->getName());
                }
            }
            if (block == func->getBlocks().back())
            {
                std::string findName = block->getName();
                if (foundRets != 1)
                {
                    InstructionNode* dummyInstRet =
                        new InstructionNode(new Token("ret", TokenType::IDENTIFIER), {});
                    func->addBlock(
                        this->genBlock(func->getName() + ".UnifiedReturn", {dummyInstRet}));
                    findName = func->getName() + ".UnifiedReturn";
                }
                ir::Block* loopBlock = block;
                if (foundRets != 1)
                {
                    loopBlock = func->getBlockByName(findName);
                }
                block = func->getBlockByName(findName);
                if (block->getInstructions().back()->getOpcode() != ir::Opcode::Ret)
                {
                    this->_diagMngr->log(
                        DiagLevel::ERROR, 0,
                        "Expected last instruction of function `%s` to be a `ret`\n", name.c_str());
                }
                std::vector<std::pair<std::string, std::string>> phiInputs;
                std::set<std::string>                            seen;
                for (std::string pred : loopBlock->getPredecessors())
                {
                    aliasses             = func->getBlockByName(pred)->getAliasses();
                    std::string ssaValue = getSSAValueFromReg("%rax", false);
                    if (seen.insert(ssaValue).second)
                    {
                        phiInputs.emplace_back(ssaValue, pred);
                    }
                }
                for (ir::Block* blk : func->getBlocks())
                {
                    for (std::pair<std::string, std::string> alias : blk->getAliasses())
                    {
                        aliasses.push_back(alias);
                    }
                }
                block->removeLastInst();
                if (phiInputs.size() > 1)
                {
                    aliasses.clear();
                    std::string newSSA       = newResult();
                    std::string aliassedName = "%rax";
                    aliasses.push_back({aliassedName, newSSA});
                    block->addAlias({aliassedName, newSSA});
                    block->addInstruction(
                        new ir::Instruction(ir::Opcode::Phi, phiInputs, newSSA, true));
                }
                block->addInstruction(new ir::Instruction(
                    ir::Opcode::Ret, {this->genOperand(new RegisterExpressionNode(
                                         new Token("rax", TokenType::IDENTIFIER)))}));
            }
        }
    }
    return func;
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
            if (declNode->getDeclType() != DeclarationNodeType::Label &&
                declNode->getDeclType() != DeclarationNodeType::Extern)
            {
                this->_diagMngr->log(DiagLevel::ICE, 0,
                                     "A declaration node with type %lu slipped trough\n",
                                     declNode->getDeclType());
            }
            if (declNode->getDeclType() == DeclarationNodeType::Label)
            {
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
                        this->_diagMngr->log(DiagLevel::ERROR, 0,
                                             "Redefinition of object body `%s`\n",
                                             sym->getName().c_str());
                    }
                    objects.insert_or_assign(sym->getName(), std::vector<AstNode*>({}));
                    currentObject = sym->getName();
                    currentFunction.clear();
                    continue;
                }
            }
            else if (declNode->getDeclType() == DeclarationNodeType::Extern)
            {
                ExternDeclarationNode* externDeclNode =
                    reinterpret_cast<ExternDeclarationNode*>(declNode);
                Symbol* sym =
                    this->_symTable->getSymbolByName(externDeclNode->getName()->get_value());
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
                        this->_diagMngr->log(DiagLevel::ERROR, 0,
                                             "Redefinition of object body `%s`\n",
                                             sym->getName().c_str());
                    }
                    objects.insert_or_assign(sym->getName(), std::vector<AstNode*>({}));
                    currentObject = sym->getName();
                    currentFunction.clear();
                    continue;
                }
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
    for (const auto& [_name, _nodes] : functions)
    {
        section->addFunction(this->genFunction(_name, _nodes));
    }
    for (const auto& [_name, _nodes] : objects)
    {
        this->_diagMngr->log(DiagLevel::WARNING, 0, "TODO: Emit object `%s`\n", _name.c_str());
    }
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
            if (declNode->getDeclType() != DeclarationNodeType::Label &&
                declNode->getDeclType() != DeclarationNodeType::Extern)
            {
                continue;
            }
        }
        sections.at(sectionName).push_back(node);
    }
    ir::Module* _module = new ir::Module;
    for (const auto& [name, nodes] : sections)
    {
        _module->addSection(this->genSection(name, nodes));
    }
    return _module;
}
Symbol* IrGen::getSymbolByName(std::string name)
{
    return this->_symTable->getSymbolByName(name);
}
ir::Block* IrGen::getCurrentBlock()
{
    return this->_currentBlock;
}
ir::Function* IrGen::getCurrentFunction()
{
    return this->_currentFunc;
}
void IrGen::setTempBlockName(std::string name)
{
    this->_tempBlockName = name;
}
}; // namespace assembler::ir::gen

// FIX Multiple rets to branch to 1 UnifiedReturn like LLVM does