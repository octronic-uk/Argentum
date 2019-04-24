#include <stdio.h>

int sprintf(char *buf, const char *fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vsprintf(&buf, fmt, ap);
	va_end(ap);

	return r;
}