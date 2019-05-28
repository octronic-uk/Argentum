#include <stdint.h>
#include <string.h>

char* strcat(char* dest, const char* src)
{
    char *ret = dest;
    while (*dest)
        dest++;
    while (*dest++ = *src++)
        ;
    return ret;
}
