#pragma once
#include "../Types.h"

#define SCREEN_ROWS 25
#define SCREEN_COLUMNS 80
#define SCREEN_BYTES_PER_ELEMENT 2
#define SCREEN_CHAR_SIZE 2000
#define SCREEN_ROW_SIZE_BYTES 160
#define SCREEN_SIZE_BYTES 4000
#define SCREEN_BUFFER_SIZE_BYTES 4000 * 128 

static char ScreenBuffer[SCREEN_BUFFER_SIZE_BYTES];
static unsigned char* ScreenVideoBasePointer = (unsigned char*)0xb8000;
static uint32_t ScreenBufferWriteOffset = 0;
static uint32_t ScreenBufferReadOffset = 0;
static int32_t ScreenScrollOffset = 0;
static uint32_t ScreenScrollOffsetMax;
static uint8_t ScreenCharAttributes = 0x07;

void tkScreen_Init();
void tkScreen_Print(const char* str);
void tkScreen_PrintLine(const char* str);
void tkScreen_Clear(void);
void tkScreen_NewLine(void);
void tkScreen_Update();
void tkScreen_SetCharAttributes(uint8_t attributes);
void tkScreen_MoveScrollOffset(int32_t offset);