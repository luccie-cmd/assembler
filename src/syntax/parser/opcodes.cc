// List of all possible opcodes in X86 family
#ifndef _OPCODES_H_
#define _OPCODES_H_
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace utils
{
std::vector<std::string> opcodes   = {"mov", "xor", "call", "jmp", "je", "ret", "cmp"};
std::vector<std::string> registers = {
    "rax",
    "rbx",
    "rcx",
    "rdi",
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
    std::printf("ICE: Invalid register `%s` brought to getRegisterSize. Sema do better.",
                reg.c_str());
    std::exit(1);
}
} // namespace utils

#endif