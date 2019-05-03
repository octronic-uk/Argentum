#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <itoa.h>
#include <stdio.h>
#include <string.h>

int printf(const char *fmt, ...)
{
	char buffer[1000];
	va_list ap;
	int r;
	va_start(ap, fmt);
	r = vsprintf(buffer, fmt, ap);
	va_end(ap);

	char* next = &buffer[0];
	while(*next)
	{
		putchar(*(next++));
	}
	return r;
}
