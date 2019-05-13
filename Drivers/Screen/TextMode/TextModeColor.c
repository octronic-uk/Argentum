#include "TextModeColor.h"

uint8_t TextModeEntryColor(struct TextModeDriver* self, enum TextModeColor fg, enum TextModeColor bg)
{
	return fg | bg << 4;
}

uint16_t TextModeEntry(struct TextModeDriver* self, unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}