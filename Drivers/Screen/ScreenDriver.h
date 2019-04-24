#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum _VgaColor
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

struct ScreenDriver
{
	uint8_t* VideoBasePointer;
	uint32_t CurrentRow;
	uint32_t CurrentColumn;
	uint8_t Color;
} ;

bool ScreenDriver_Constructor(struct ScreenDriver* self);
void ScreenDriver_PutChar(struct ScreenDriver* self, char c);
void ScreenDriver_Write(struct ScreenDriver* self, const char* data, uint32_t size);
void ScreenDriver_WriteString(struct ScreenDriver* self, const char* data);
void ScreenDriver_Update(struct ScreenDriver* self);
void ScreenDriver_Clear(struct ScreenDriver* self);
void ScreenDriver_Scroll(struct ScreenDriver* self);
uint8_t _VgaEntryColor(struct ScreenDriver* self, enum _VgaColor fg, enum _VgaColor bg);
uint16_t _VgaEntry(struct ScreenDriver* self, unsigned char uc, uint8_t color);
void ScreenDriver_SetHeader(struct ScreenDriver* self, const char* data);
void ScreenDriver_SetCursorPosition(struct ScreenDriver* self, uint8_t column, uint8_t row);
void ScreenDriver_ClearRow(struct ScreenDriver* self, uint8_t row);