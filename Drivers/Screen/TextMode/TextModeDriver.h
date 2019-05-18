#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct TextModeDriver
{
	uint8_t* VideoBasePointer;
	uint32_t Width;
	uint32_t Height;
	uint32_t CurrentRow;
	uint32_t CurrentColumn;
	uint8_t Color;
	bool EnableEcho;
	bool Available;
} ;
typedef struct TextModeDriver TextModeDriver;

bool TextModeDriver_Constructor(TextModeDriver* self);
void TextModeDriver_PutChar(TextModeDriver* self, char c);
void TextModeDriver_Write(TextModeDriver* self, const char* data, uint32_t size);
void TextModeDriver_WriteString(TextModeDriver* self, const char* data);
void TextModeDriver_Update(TextModeDriver* self);
void TextModeDriver_Clear(TextModeDriver* self);
void TextModeDriver_Scroll(TextModeDriver* self);

void TextModeDriver_SetHeader(TextModeDriver* self, const char* data);
void TextModeDriver_SetCursorPosition(TextModeDriver* self, uint8_t column, uint8_t row);
void TextModeDriver_ClearRow(TextModeDriver* self, uint8_t row);
void TextModeDriver_SetFramebufferCursorPosition(TextModeDriver* self, uint16_t position);
void TextModeDriver_SetEcho(TextModeDriver* self, bool echo);
char TextModeDriver_ReadCharacter(TextModeDriver* self);
uint32_t TextModeDriver_ReadString(TextModeDriver* self, char* buffer, uint32_t size);

// VGA Functions
