#pragma once

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

int      memcmp(const void*, const void*, uint32_t);
void*    memcpy(void* __restrict, const void* __restrict, uint32_t);
void*    memmove(void*, const void*, uint32_t);
void*    memset(void*, int, uint32_t);
void*    memchr(const char* ptr, int value, uint32_t num);

uint32_t strlen(const char*);
int      strncmp(const char *s1, const char *s2, uint32_t n);
int      strcmp(const char* s1, const char* s2);
char*    strcat(char* dest, const char* src);
char*    strncpy(char *dest, const char* src, uint32_t count);
char*    strncat(char* destination, const char* source, uint32_t num);
char*    strtok(char* str, const char* delim);
uint32_t strxfrm( char* s1, const char* s2, uint32_t n);
char*    strchr(const char* str, int character);
uint32_t strspn(const char* str1, const char* str2);
uint32_t strcspn(const char* str1, const char* str2);
char*    strrchr(const char* str, int character);
char*    strerror(int errnum);
char*    strstr(const char* str1, const char* str2);
char*    strpbrk(const char* str1, const char* str2);
char*    strcpy(char* destination, const char* source);
int      strcoll(const char* str1, const char * str2);
char*    strdup(const char *str1);


int atoi(const char *nptr);
int is_space(char ch);
int chrtoi(char ch, int base);