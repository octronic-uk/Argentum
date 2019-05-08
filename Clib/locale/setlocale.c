#include <locale.h>
#include <stdio.h>
char* setlocale (int arg0, const char* arg1)
{
    printf("stdlib: setlocale\n");
    return 0;
}