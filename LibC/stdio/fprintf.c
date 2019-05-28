#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int fprintf(FILE* stream, const char* fmt, ...)
{ 
	/*
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vsprintf(NULL, fmt, ap);
	va_end(ap);
	return r;
	*/

// TODO - direct to FILE* stream  use stdout for now
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
	putchar('\n');
	return r;
}