#include <stdlib.h>
#include <stdio.h>

__attribute__((__noreturn__)) void exit(int status)
{
    printf("stdlib: exit(%d)\n",status);
    abort();
}