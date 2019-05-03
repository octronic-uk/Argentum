#include <stdio.h>
#include <stdint.h>

char* fgets(char* str, int num, FILE* stream)
{
    uint32_t i;
    for (i=0; i<num-1; i++)
    {
        str[i] = getchar();
        if (str[i] == 0 || str[i] == '\n') break;
    }
    str[i+1] = 0;
    return str;
}