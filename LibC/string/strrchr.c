#include <string.h>

char *strrchr(const char *s, int c)
{
    char* ret=0;
    do {
        if( *s == (char)c )
            ret=(char*)s;
    } while(*s++);
    return ret;
}