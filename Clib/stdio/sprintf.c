#include <stdio.h>

int sprintf(char *s, const char *fmt, ...)
{
	int i;
	va_list Args;
	va_start(Args,fmt);
	i=vsprintf(s,fmt,Args);
	va_end(Args);
	return i;
}