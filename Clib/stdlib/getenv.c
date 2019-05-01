#include <stdlib.h>

char* getenv(const char* name)
{
    printf("stdlib: getenv %s\n",name);
    return "env_value";
}