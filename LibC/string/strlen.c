#include <string.h>

uint32_t strlen(const char* s)
{
	const char *p = s;
	while (*s) ++s;
	return s - p;
}
