#include <stdio.h>
#include <string.h>

int fputs ( const char * str, FILE * stream )
{
    //printf("fputs: %s",str);
    printf("%s",str);
    return strlen(str);
}