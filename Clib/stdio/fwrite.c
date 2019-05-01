#include <stdio.h>

uint32_t fwrite(const void* ptr, uint32_t size, uint32_t count, FILE* stream)
{
    //printf("stdlib: fwrite %s\n",(const char*)ptr);
    int i;
    const char* char_ptr = (const char*)ptr;
    for (i=0; i< count; i++)
    {
        putchar(char_ptr[i]);
    }
    return 0;
}