#if !defined(_ASSEMBLER_DRIVER_DIAG_H_)
#define _ASSEMBLER_DRIVER_DIAG_H_
#include <string>

namespace assembler
{
enum struct DiagLevel : int
{
    NOTE,
    INFO,
    WARNING,
    ERROR,
    ICE,
};
std::string diagLvLToColorWithName(DiagLevel lvl);
class DiagManager
{
  public:
    DiagManager(std::string file, bool useColors, bool exitOnError);
    ~DiagManager();
    void vlog(DiagLevel lvl, size_t offset, const char* fmt, va_list args);
    void log(DiagLevel lvl, size_t offset, const char* fmt, ...);

  private:
    std::string _file;
    bool        _useColors, _exitOnError;
};
} // namespace assembler

#endif // _ASSEMBLER_DRIVER_DIAG_H_
