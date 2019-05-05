#include <string.h>

char* strcpy(char* dest, const char* src )
{
    char* tmp = dest;
    while (*tmp++ = *src++);
    return dest;
}