#include <Drivers/Screen/ScreenDriver.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define SCREEN_VBP 0x000B8000
#define SCREEN_TAB_SIZE 4
#define SCREEN_ROWS 25
#define SCREEN_COLUMNS 80
#define SCREEN_BYTES_PER_ELEMENT 2
#define SCREEN_CHAR_SIZE (SCREEN_ROWS * SCREEN_COLUMNS)
#define SCREEN_ROW_SIZE_BYTES (SCREEN_COLUMNS * SCREEN_BYTES_PER_ELEMENT)
#define SCREEN_SIZE_BYTES (SCREEN_CHAR_SIZE * SCREEN_BYTES_PER_ELEMENT)

bool ScreenDriver_Constructor(struct ScreenDriver* self, struct Kernel* kernel)
{
	self->Kernel = kernel;
	self->VideoBasePointer = (uint8_t*)SCREEN_VBP;
	self->CurrentRow = 0;
	self->CurrentColumn = 0;
	self->CurrentRow = 0;
	self->CurrentColumn = 0;
	self->Color = _VgaEntryColor(self, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	ScreenDriver_Clear(self);
	return true;
}

void ScreenDriver_SetColor(struct ScreenDriver* self, uint8_t color)
{
	self->Color = color;
}

void ScreenDriver_PutEntryAt(struct ScreenDriver* self, unsigned char c, uint8_t color, size_t x, size_t y)
{
	size_t index = (y * SCREEN_COLUMNS * SCREEN_BYTES_PER_ELEMENT) + (x*SCREEN_BYTES_PER_ELEMENT);
	self->VideoBasePointer[index] = c;
	self->VideoBasePointer[index+1] = color;
}

void ScreenDriver_PutChar(struct ScreenDriver* self, char c)
{
	unsigned char uc = c;
    if (c == '\n')
    {
        self->CurrentRow++;
        self->CurrentColumn = 0;
		if (self->CurrentRow >= SCREEN_ROWS)
		{
			ScreenDriver_Scroll(self);
		}
        return;
    }

    if (c == '\t')
    {
        self->CurrentColumn += SCREEN_TAB_SIZE;
        return;
    }

	ScreenDriver_PutEntryAt(self, uc, self->Color, self->CurrentColumn, self->CurrentRow);
	if (++self->CurrentColumn == SCREEN_COLUMNS)
    {
		self->CurrentColumn = 0;
		if (++self->CurrentRow == SCREEN_ROWS)
        {
			self->CurrentRow = 0;
        }
	}
    
}

void ScreenDriver_Write(struct ScreenDriver* self, const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
    {
		ScreenDriver_PutChar(self, data[i]);
    }
}

void ScreenDriver_WriteString(struct ScreenDriver* self, const char* data)
{
	ScreenDriver_Write(self, data, strlen(data));
}

void ScreenDriver_Clear(struct ScreenDriver* self)
{
	for (size_t y = 0; y < SCREEN_ROWS; y++)
    {
		for (size_t x = 0; x < SCREEN_COLUMNS; x++)
        {
			const size_t index = y * SCREEN_COLUMNS + x;
			ScreenDriver_PutEntryAt(self, ' ',self->Color, x,y);
		}
	}
	self->CurrentColumn = 0;
	self->CurrentRow = 0;
}

void ScreenDriver_Scroll(struct ScreenDriver* self)
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

uint8_t _VgaEntryColor(struct ScreenDriver* self, enum _VgaColor fg, enum _VgaColor bg)
{
	return fg | bg << 4;
}

uint16_t _VgaEntry(struct ScreenDriver* self, unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}