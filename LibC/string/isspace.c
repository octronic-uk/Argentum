#include <string.h>

int isspace(int ch)
{
    if (ch == ' ')  return 1; // (0x20) space (SPC)
    if (ch == '\t') return 1; // (0x09) horizontal tab (TAB)
    if (ch == '\n') return 1; // (0x0a) newline (LF)
    if (ch == '\v') return 1; // (0x0b) vertical tab (VT)
    if (ch == '\f') return 1; // (0x0c) feed (FF)
    if (ch == '\r') return 1; // (0x0d) carriage return (CR)
    return 0;
}