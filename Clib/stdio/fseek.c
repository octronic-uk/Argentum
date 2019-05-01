#include <stdio.h>

int fseek(FILE* stream, long int offset, int origin)
{
    printf("stdlib: fseek\n");
    return 0;
}