#include <stdlib.h>

unsigned long int strtoul (const char* str, char** endptr, int base)
{
    return (unsigned long int)strtol(str, endptr, base);
}