#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

__attribute__((__noreturn__)) void abort(void);
int abs(int n);

void* malloc(uint32_t size);
void* realloc(void* ptr, uint32_t size);
void free(void*);
char* getenv (const char* name);
double strtod (const char* str, char** endptr);
void exit (int status);
int system (const char* command);

#define EXIT_SUCCESS 0 
#define EXIT_FAILURE 1
