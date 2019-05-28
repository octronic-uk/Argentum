#include <stdio.h>

int getc(FILE* stream)
{
    printf("getc:\n");
    return getchar();
}