#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    int memcmp(const void*, const void*, size_t);
    void* memcpy(void* __restrict, const void* __restrict, size_t);
    void* memmove(void*, const void*, size_t);
    void* memset(void*, int, size_t);
    size_t strlen(const char*);
    int strncmp(const char *s1, const char *s2, size_t n);
    int strcmp(const char* s1, const char* s2);
    char* strcat (char* dest, const char* src);
    char *strncpy(char *dest, const char *src, size_t count);
    long int strtol (const char* nptr, char** endptr, int base);
    int atoi (const char *nptr);
    int _isspace(char ch);
    int chartoi(char ch, int base);
    //const char * strchr ( const char * str, int character );
    char* strchr (char * str, int character);
    size_t strspn (const char * str1, const char * str2);
#ifdef __cplusplus
}
#endif

#endif
