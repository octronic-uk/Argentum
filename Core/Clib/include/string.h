#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    int   memory_compare(const void*, const void*, size_t);
    void* memory_copy(void* __restrict, const void* __restrict, size_t);
    void* memory_move(void*, const void*, size_t);
    void* memory_set(void*, int, size_t);

    size_t string_length(const char*);
    int    string_n_compare(const char *s1, const char *s2, size_t n);
    int    string_compare(const char* s1, const char* s2);
    char*  string_concatenate(char* dest, const char* src);
    char*  string_n_copy(char *dest, const char *src, size_t count);
    long int string_to_long(const char* nptr, char** endptr, int base);
    char* string_char(char * str, int character);
    size_t string_charsapn(const char * str1, const char * str2);

    int ascii_to_int(const char *nptr);
    int is_space(char ch);
    int char_to_int(char ch, int base);
#ifdef __cplusplus
}
#endif

// std c compatibility
#define memcpy  memory_compare
#define memmove memory_move
#define memset  memory_set

#define strlen  string_length
#define strncmp string_n_compare
#define strcmp  string_compare
#define strcat  string_concatenate
#define strncpy string_n_copy
#define strtol  string_to_long
#define strchr  string_char
#define strspn  string_charspan

#define atoi    ascii_to_int
#define chartoi  char_to_int

#endif
