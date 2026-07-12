#include <cstdarg>
#include "SEGGER_RTT.h"

extern "C" int SEGGER_RTT_vprintf(unsigned BufferIndex, const char * sFormat, va_list * pParamList);

extern "C"
int xprintf(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    int len = SEGGER_RTT_vprintf(0, format, &arg);
    va_end(arg);
    return len;
}
