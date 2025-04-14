// List of all possible opcodes in X86 family
#ifndef _OPCODES_H_
#define _OPCODES_H_
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace utils
{
std::vector<std::string> opcodes   = {"mov", "xor",  "call", "jmp", "je",    "jl",  "ret",
                                      "cmp", "push", "add",  "shl", "cmove", "pop", "inc"};
std::vector<std::string> registers = {
    "rax", "eax",  "ax",   "al",   "ah",  "rbx",  "ebx",  "bx",   "bl",  "bh",   "rcx",  "ecx",
    "cx",  "cl",   "ch",   "rdx",  "edx", "dx",   "dl",   "dh",   "rdi", "edi",  "di",   "dil",
    "rsi", "esi",  "si",   "sil",  "rbp", "ebp",  "bp",   "bpl",  "rsp", "esp",  "sp",   "spl",
    "r8",  "r8d",  "r8w",  "r8b",  "r9",  "r9d",  "r9w",  "r9b",  "r10", "r10d", "r10w", "r10b",
    "r11", "r11d", "r11w", "r11b", "r12", "r12d", "r12w", "r12b", "r13", "r13d", "r13w", "r13b",
    "r14", "r14d", "r14w", "r14b", "r15", "r15d", "r15w", "r15b",
};
bool takesArgument(std::string opcode)
{
    static std::vector<std::string> nonArgMnemonics = {
        "ret",
    };
    if (std::find(nonArgMnemonics.begin(), nonArgMnemonics.end(), opcode) == nonArgMnemonics.end())
    {
        return true;
    }
    return false;
}
uint8_t getRegisterSize(std::string reg)
{
    if (std::find(registers.begin(), registers.end(), reg) == registers.end())
    {
        std::printf("ICE: Invalid register `%s` brought to getRegisterSize. Sema do better.",
                    reg.c_str());
        std::exit(1);
    }
    if (reg.starts_with("r"))
    {
        if (reg.ends_with("b"))
            return 8;
        if (reg.ends_with("w"))
            return 16;
        if (reg.ends_with("d"))
            return 32;
        return 64;
    }
    if (reg.starts_with("e"))
    {
        return 32;
    }
    std::printf("ICE: Invalid register `%s` brought to getRegisterSize. Sema do better.",
                reg.c_str());
    std::exit(1);
}
bool getBranchInstruction(std::string mnemonic)
{
    static std::vector<std::string> branchInsts = {
        "jmp", "call", "je", "jl", "jg", "jle", "jge", "jne",
    };
    return std::find(branchInsts.begin(), branchInsts.end(), mnemonic) != branchInsts.end();
}
} // namespace utils

#endif