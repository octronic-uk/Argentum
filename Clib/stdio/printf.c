#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <itoa.h>
#include <stdio.h>
#include <string.h>

int printf(const char *fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vsprintf(NULL, fmt, ap);
	va_end(ap);
	return r;
}
