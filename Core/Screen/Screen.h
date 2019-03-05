#pragma once

/* there are 25 lines each of 80 columns; each element takes 2 bytes */
#define SCREEN_LINES 25
#define SCREEN_COLUMNS 80
#define SCREEN_BYTES_FOR_EACH_ELEMENT 2
#define SCREEN_LINE_SIZE SCREEN_BYTES_FOR_EACH_ELEMENT * SCREEN_COLUMNS
#define SCREEN_SIZE SCREEN_BYTES_FOR_EACH_ELEMENT * SCREEN_COLUMNS * SCREEN_LINES

static unsigned int screenCursorLocation = 0;
void tkScreenPrint(const char* str);
void tkScreenPrintLine(const char* str);
void tkScreenClear(void);
void tkScreenNewLine(void);