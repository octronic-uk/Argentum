#include <string.h>
#include <stdio.h>
void* memchr(const char* s, int c, uint32_t n)
{
    unsigned char *p = (unsigned char*)s;
    while( n-- )
        if( *p != (unsigned char)c )
            p++;
        else
            return p;
    return 0;
}