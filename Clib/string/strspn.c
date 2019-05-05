#include <string.h>

uint32_t strspn(const char * s1, const char * s2)
{
    uint32_t ret=0;
    while(*s1 && strchr(s2,*s1++)) ret++;
    return ret; 
}