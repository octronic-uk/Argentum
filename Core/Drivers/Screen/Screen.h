#pragma once

#include <stddef.h>
#include <stdint.h>

enum Screen_VgaColor
{
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

uint8_t Screen_VgaEntryColor(enum Screen_VgaColor fg, enum Screen_VgaColor bg);
uint16_t Screen_VgaEntry(unsigned char uc, uint8_t color);

void Screen_Initialize(void);
void Screen_PutChar(char c);
void Screen_Write(const char* data, size_t size);
void Screen_WriteString(const char* data);
void Screen_Update();
void Screen_Clear();
void Screen_Scroll();