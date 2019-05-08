#include <stdio.h>

int setvbuf(FILE* stream, char* buffer, int mode, uint32_t size )
{
    printf("stdlib: setvbuf\n");
    return 0;
}
