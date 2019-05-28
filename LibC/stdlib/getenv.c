#include <stdlib.h>
#include <stdio.h>
char* getenv(const char* name)
{
    printf("getenv %s\n",name);
    return "env_value";
}