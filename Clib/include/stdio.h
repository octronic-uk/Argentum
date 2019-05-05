#pragma once

#include <sys/cdefs.h>
#include <stdarg.h>
#include <errno.h>
#include <stdint.h>

#define BUFSIZ 64
#define EOF (-1)
#define L_tmpnam 11
#define SEEK_SET 0
#define SEEK_CUR 0
#define SEEK_END 0
#define _IONBF 0
#define _IOFBF 0
#define _IOLBF 0

enum flags 
{
	PAD_ZERO	= 1,
	PAD_RIGHT	= 2,
};

typedef struct
{
    uint32_t p;
} FILE;

static FILE* stdin;
static FILE* stdout;
static FILE* stderr;


int   remove(const char* filename);
int   rename(const char* old_filename, const char* new_filename);
char* tmpnam(char *str);
void  outputchar(char *str, char c);
int   outputi(char *out, long i, int base, int sign, int width, int flags, int letbase);
int   prints(char *out, const char *string, int width, int flags);
int   vsprintf(char *out, const char *format, va_list ap);
int   sprintf(char *buf, const char *fmt, ...);
int   snprintf ( char * s, uint32_t n, const char * format, ... );
int   sscanf ( const char * s, const char * format, ...);
int   printf(const char* __restrict, ...);
int   putchar(int);
int   puts(const char*);
void perror ( const char * str );
// File API

void     clearerr(FILE* stream );
int      fclose(FILE* stream);
int      feof(FILE* stream);
int      ferror(FILE* stream);
int      fflush ( FILE * stream );
int 	 fgetc ( FILE * stream );
char*    fgets ( char * str, int num, FILE * stream );
FILE*    fopen(const char* filename, const char* mode );
uint32_t fread(void* ptr, uint32_t size, uint32_t count, FILE* stream);
FILE*    freopen(const char* filename, const char* mode, FILE* stream);
int      fprintf(FILE* stream, const char* format, ...);
int 	 fputc ( int character, FILE * stream );
int      fputs ( const char * str, FILE * stream );
int      fscanf ( FILE * stream, const char * format, ... );
int      fseek ( FILE * stream, long int offset, int origin );
long int ftell ( FILE * stream );
uint32_t fwrite ( const void * ptr, uint32_t size, uint32_t count, FILE * stream );
int      getc(FILE* stream);
int      getchar();
char*    gets(char* str);
int      setvbuf(FILE* stream, char* buffer, int mode, uint32_t size );
FILE*    tmpfile();
int      ungetc(int character, FILE * stream );



