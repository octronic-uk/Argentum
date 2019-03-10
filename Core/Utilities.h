#pragma once

#define BUFLEN 128
#define BASE_10 10
#define BASE_16 16

void swap(char *x, char *y);
void reverse(char *buffer, int i, int j);
void itoa(int value, char* buffer, int base);
void ultoa(unsigned long value, char* buffer, int base);
void* memset( void* dst, int ch, unsigned long count );
unsigned long strlen(const char* str);
int memcpy(void* dst, const void* src, int size);