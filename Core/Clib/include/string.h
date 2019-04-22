#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

int   memory_compare(const void*, const void*, uint32_t);
void* memory_copy(void* __restrict, const void* __restrict, uint32_t);
void* memory_move(void*, const void*, uint32_t);
void* memory_set(void*, int, uint32_t);
void* memory_char(void* ptr, int value, uint32_t num);

uint32_t string_length(const char*);
int      string_n_compare(const char *s1, const char *s2, uint32_t n);
int      string_compare(const char* s1, const char* s2);
char*    string_concatenate(char* dest, const char* src);
char*    string_n_copy(char *dest, const char *src, uint32_t count);
long int string_to_long(const char* nptr, char** endptr, int base);
char*    string_char(const char * str, int character);
uint32_t string_charspan(const char * str1, const char * str2);
char*    string_error( int errnum );
char*    string_within_string(const char* str1, const char* str2 );
char* strpbrk(const char* str1, const char* str2 );
char* strcpy(const char* destination, const char* source );
int strcoll ( const char * str1, const char * str2 );

int ascii_to_int(const char *nptr);
int is_space(char ch);
int char_to_int(char ch, int base);


// Useful to annoying name mappings because legacy

#define memcmp  memory_compare
#define memcpy  memory_copy
#define memmove memory_move
#define memset  memory_set
#define memchr  memory_char

#define strlen   string_length
#define strncmp  string_n_compare
#define strcmp   string_compare
#define strcat   string_concatenate
#define strncpy  string_n_copy
#define strtol   string_to_long
#define strchr   string_char
#define strspn   string_charspan
#define strerror string_error
#define strstr   string_within_string

#define atoi     ascii_to_int

#define chartoi  char_to_int

#endif
