#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Drivers/Screen/Screen.h>

#define SCREEN_VBP 0x000B8000
#define SCREEN_TAB_SIZE 4
#define SCREEN_ROWS 25
#define SCREEN_COLUMNS 80
#define SCREEN_BYTES_PER_ELEMENT 2
#define SCREEN_CHAR_SIZE (SCREEN_ROWS * SCREEN_COLUMNS)
#define SCREEN_ROW_SIZE_BYTES (SCREEN_COLUMNS * SCREEN_BYTES_PER_ELEMENT)
#define SCREEN_SIZE_BYTES (SCREEN_CHAR_SIZE * SCREEN_BYTES_PER_ELEMENT)

void Screen_Constructor(struct Screen* self)
{
	self->VideoBasePointer = (uint8_t*)SCREEN_VBP;
	self->CurrentRow = 0;
	self->CurrentColumn = 0;
	self->CurrentRow = 0;
	self->CurrentColumn = 0;
	self->Color = Screen_VgaEntryColor(self, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	Screen_Clear(self);
}

void Screen_SetColor(struct Screen* self, uint8_t color)
{
	self->Color = color;
}

void Screen_PutEntryAt(struct Screen* self, unsigned char c, uint8_t color, size_t x, size_t y)
{
	size_t index = (y * SCREEN_COLUMNS * SCREEN_BYTES_PER_ELEMENT) + (x*SCREEN_BYTES_PER_ELEMENT);
	self->VideoBasePointer[index] = c;
	self->VideoBasePointer[index+1] = color;
}

void Screen_PutChar(struct Screen* self, char c)
{
	unsigned char uc = c;
    if (c == '\n')
    {
        self->CurrentRow++;
        self->CurrentColumn = 0;
		if (self->CurrentRow >= SCREEN_ROWS)
		{
			Screen_Scroll(self);
		}
        return;
    }

    if (c == '\t')
    {
        self->CurrentColumn += SCREEN_TAB_SIZE;
        return;
    }

	Screen_PutEntryAt(self, uc, self->Color, self->CurrentColumn, self->CurrentRow);
	if (++self->CurrentColumn == SCREEN_COLUMNS)
    {
		self->CurrentColumn = 0;
		if (++self->CurrentRow == SCREEN_ROWS)
        {
			self->CurrentRow = 0;
        }
	}
    
}

void Screen_Write(struct Screen* self, const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
    {
		Screen_PutChar(self, data[i]);
    }
}

void Screen_WriteString(struct Screen* self, const char* data)
{
	Screen_Write(self, data, strlen(data));
}

void Screen_Clear(struct Screen* self)
{
	for (size_t y = 0; y < SCREEN_ROWS; y++)
    {
		for (size_t x = 0; x < SCREEN_COLUMNS; x++)
        {
			const size_t index = y * SCREEN_COLUMNS + x;
			Screen_PutEntryAt(self, ' ',self->Color, x,y);
		}
	}
	self->CurrentColumn = 0;
	self->CurrentRow = 0;
}

uint8_t Screen_VgaEntryColor(struct Screen* self, enum Screen_VgaColor fg, enum Screen_VgaColor bg)
{
	return fg | bg << 4;
}

uint16_t Screen_VgaEntry(struct Screen* self, unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void Screen_Scroll(struct Screen* self)
{
	int row;
	for (row = 1; row < SCREEN_ROWS; row++)
	{
		memcpy(
			&self->VideoBasePointer[SCREEN_ROW_SIZE_BYTES*(row-1)], 
			&self->VideoBasePointer[SCREEN_ROW_SIZE_BYTES*row],
			SCREEN_ROW_SIZE_BYTES
		);
		memset(
			&self->VideoBasePointer[SCREEN_ROW_SIZE_BYTES*row],
			0,
			SCREEN_ROW_SIZE_BYTES
		);
	}
	self->CurrentRow = SCREEN_ROWS-1;
}