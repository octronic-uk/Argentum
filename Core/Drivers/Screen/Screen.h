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

struct Screen 
{
	uint8_t* VideoBasePointer;
	size_t   CurrentRow;
	size_t   CurrentColumn;
	uint8_t  Color;
} ;

void Screen_Constructor(struct Screen* self);
void Screen_PutChar(struct Screen* self, char c);
void Screen_Write(struct Screen* self, const char* data, size_t size);
void Screen_WriteString(struct Screen* self, const char* data);
void Screen_Update(struct Screen* self);
void Screen_Clear(struct Screen* self);
void Screen_Scroll(struct Screen* self);
uint8_t Screen_VgaEntryColor(struct Screen* self, enum Screen_VgaColor fg, enum Screen_VgaColor bg);
uint16_t Screen_VgaEntry(struct Screen* self, unsigned char uc, uint8_t color);