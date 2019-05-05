#include <string.h>

uint32_t strxfrm(char* s1, const char* s2, uint32_t n)
{
    uint32_t len = strlen( s2 );
    if ( len < n )
    {
        while ( n-- && ( *s1++ = (unsigned char)*s2++ ) );
    }
    return len;
}