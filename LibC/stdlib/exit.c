#include <stdlib.h>
#include <stdio.h>

__attribute__((__noreturn__)) void exit(int status)
{
    printf("exit(%d)\n",status);
    abort();
}