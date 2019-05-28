#include <stdio.h>
#include <stdint.h>

char* gets(char* str)
{
    uint32_t i = 0;
    while(1)
    {
        char next = getchar();
        if (next == '\n')
        {
            str[i] = 0;
            break;
        }
        i++;
    }
    return str;
}