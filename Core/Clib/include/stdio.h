#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <stdarg.h>

struct Kernel;
#define BUFSIZ 64
#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int outputi(char **out, long i, int base, int sign, int width, int flags, int letbase);
int prints(char **out, const char *string, int width, int flags);
int vsprintf(char **out, const char *format, va_list ap);
int sprintf(char *buf, const char *fmt, ...);
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
void printf_to_serial();
void printf_SetKernel(struct Kernel* kernel);

#ifdef __cplusplus
}
#endif

#endif


