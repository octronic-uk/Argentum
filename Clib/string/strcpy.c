#include <string.h>

char* strcpy(const char* destination, const char* source )
{
    char* src = (char*)source;
    char* dst = (char*)destination;

    do
    {
        *dst++ = *src++;
    }
    while(*src);

    return 0;
}