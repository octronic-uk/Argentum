#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

#define ITOA_BUFFER_LEN 16
#define EXIT_SUCCESS 0 
#define EXIT_FAILURE 1

static char ItoaBuffer[ITOA_BUFFER_LEN];

__attribute__((__noreturn__)) void abort_debug(char* file, uint32_t line);

#define abort() abort_debug(__FILE__, __LINE__)

int abs(int n);

void* _malloc(uint32_t size, char* file, uint32_t line);
void* _realloc(void* ptr, uint32_t size, char* file, uint32_t line);
void  _free(void* ptr, char* file, uint32_t line);

#define malloc(sz)       _malloc (sz,      __FILE__, __LINE__)
#define realloc(ptr, sz) _realloc(ptr, sz, __FILE__, __LINE__)
#define free(ptr)        _free   (ptr,     __FILE__, __LINE__)


int system (const char* command);
void exit (int status);

void swap(char* a, char* b);
void reverse(char *buffer, int i, int j);

char*  getenv (const char* name);
double strtod (const char* str, char** endptr);

void   itoa(int value, char* str, int base);
void   ultoa(unsigned long value, char* str, int base);
uint32_t strtoul (const char* str, char** endptr, int base);
int32_t  strtol(const char* nptr, char** endptr, int base);

void qsort (void* base, uint32_t num, uint32_t size, int (*compar)(const void*,const void*));
