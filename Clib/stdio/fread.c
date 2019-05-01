#include <stdio.h>

uint32_t fread(void* ptr, uint32_t size, uint32_t count, FILE* stream)
{
    printf("stdlib: fread\n");
    return 0;
}