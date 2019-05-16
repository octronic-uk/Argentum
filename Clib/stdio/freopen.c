#include <stdio.h>


FILE* freopen(const char* filename, const char* mode, FILE* stream)
{
    printf("freopen: filename: %s mode %s\n",filename, mode);
    return 0;
}