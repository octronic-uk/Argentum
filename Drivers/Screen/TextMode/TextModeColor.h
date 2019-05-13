#pragma once

#include <stdint.h>

struct TextModeDriver;

enum TextModeColor
{
	TM_COLOR_BLACK = 0,
	TM_COLOR_BLUE = 1,
	TM_COLOR_GREEN = 2,
	TM_COLOR_CYAN = 3,
	TM_COLOR_RED = 4,
	TM_COLOR_MAGENTA = 5,
	TM_COLOR_BROWN = 6,
	TM_COLOR_LIGHT_GREY = 7,
	TM_COLOR_DARK_GREY = 8,
	TM_COLOR_LIGHT_BLUE = 9,
	TM_COLOR_LIGHT_GREEN = 10,
	TM_COLOR_LIGHT_CYAN = 11,
	TM_COLOR_LIGHT_RED = 12,
	TM_COLOR_LIGHT_MAGENTA = 13,
	TM_COLOR_LIGHT_BROWN = 14,
	TM_COLOR_WHITE = 15,
};

uint8_t TextModeEntryColor(struct TextModeDriver* self, enum TextModeColor fg, enum TextModeColor bg);
uint16_t TextModeEntry(struct TextModeDriver* self, unsigned char uc, uint8_t color);