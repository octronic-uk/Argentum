#include <string.h>

char* skipwhite(const char *q)
{
    char	*p = (char*)q;
    while (isspace(*p))
	++p;
    return p;
}