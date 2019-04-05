#pragma once

#define ITOA_BUFFER_LEN 16

static char ItoaBuffer[ITOA_BUFFER_LEN];

void itoa(int value, char* str, int base);
void ultoa(unsigned long value, char* str, int base);
void swap(char* a, char* b);
void reverse(char *buffer, int i, int j);
