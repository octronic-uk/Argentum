#include <string.h>
#include <stdlib.h>

char *strdup(const char *s)
{
    size_t slen = strlen(s);
    char *out = malloc(slen+1);
    if (!out)
        return NULL;
    strcpy(out,s);
    return out;
}