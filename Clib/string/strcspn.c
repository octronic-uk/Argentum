#include <string.h>
#include <stdio.h>

uint32_t strcspn ( const char * s1, const char * s2 )
{
    uint32_t ret=0;
    while(*s1)
        if(strchr(s2,*s1))
            return ret;
        else
            s1++,ret++;
    return ret;
}
