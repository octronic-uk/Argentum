#include <stdlib.h>
#include <stdio.h>
char* getenv(const char* name)
{
    printf("stdlib: getenv %s\n",name);
    return "env_value";
}