#include <cstdarg>
#include <cstring>
#include <driver/ansi.h>
#include <driver/diag.h>

namespace assembler
{
std::string diagLvLToColorWithName(DiagLevel lvl)
{
    switch (lvl)
    {
    case DiagLevel::NOTE:
    {
        return COLOR_RESET + "Note: ";
    }
    break;
    case DiagLevel::INFO:
    {
        return COLOR_RESET + "Info: ";
    }
    break;
    case DiagLevel::WARNING:
    {
        return COLOR_BOLD + COLOR_PURPLE + "Warning: ";
    }
    break;
    case DiagLevel::ERROR:
    {
        return COLOR_BOLD + COLOR_RED + "Error: ";
    }
    break;
    case DiagLevel::ICE:
    {
        return COLOR_BOLD + COLOR_RED + "Ice: ";
    }
    break;
    }
    std::printf("ERROR: Invalid level type %ld\n", (std::size_t)lvl);
    std::exit(1);
}
DiagManager::DiagManager(std::string file, bool useColors, bool exitOnError)
{
    this->_file        = file;
    this->_useColors   = useColors;
    this->_exitOnError = exitOnError;
}
DiagManager::~DiagManager() {}
void DiagManager::vlog(DiagLevel lvl, size_t offset, const char* fmt, va_list args)
{
    std::string diagLvl = diagLvLToColorWithName(lvl);
    char*       buffer  = (char*)malloc(8192);
    std::vsnprintf(buffer, 8192, fmt, args);
    std::printf("%lu:%s%s%s", offset, diagLvl.c_str(), COLOR_RESET.c_str(), buffer);
    free(buffer);
    if (lvl >= DiagLevel::ERROR && this->_exitOnError)
    {
        std::exit(1);
    }
}
void DiagManager::log(DiagLevel lvl, size_t offset, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    this->vlog(lvl, offset, fmt, args);
    va_end(args);
}
}; // namespace assembler