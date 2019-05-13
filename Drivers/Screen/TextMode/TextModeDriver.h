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

bool TextModeDriver_Constructor(struct TextModeDriver* self);
void TextModeDriver_PutChar(struct TextModeDriver* self, char c);
void TextModeDriver_Write(struct TextModeDriver* self, const char* data, uint32_t size);
void TextModeDriver_WriteString(struct TextModeDriver* self, const char* data);
void TextModeDriver_Update(struct TextModeDriver* self);
void TextModeDriver_Clear(struct TextModeDriver* self);
void TextModeDriver_Scroll(struct TextModeDriver* self);

void TextModeDriver_SetHeader(struct TextModeDriver* self, const char* data);
void TextModeDriver_SetCursorPosition(struct TextModeDriver* self, uint8_t column, uint8_t row);
void TextModeDriver_ClearRow(struct TextModeDriver* self, uint8_t row);
void TextModeDriver_SetFramebufferCursorPosition(struct TextModeDriver* self, uint16_t position);
void TextModeDriver_SetEcho(struct TextModeDriver* self, bool echo);
char TextModeDriver_ReadCharacter(struct TextModeDriver* self);
uint32_t TextModeDriver_ReadString(struct TextModeDriver* self, char* buffer, uint32_t size);

// VGA Functions
