#include <stdio.h>
#include <stdint.h>
#include <string.h>
char* fgets(char* str, int num, FILE* stream)
{
    uint32_t i;
    memset(str,0,num);

    for (i=0; i<num-1; i++)
    {
        str[i] = getchar();
        if (str[i] == 0 || str[i] == '\n') break;
    }
    str[num-1] = 0;
    return str;
}