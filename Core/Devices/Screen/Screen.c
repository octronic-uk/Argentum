#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Devices/Screen/Screen.h>

#define SCREEN_TAB_SIZE 4
#define SCREEN_ROWS 25
#define SCREEN_COLUMNS 80
#define SCREEN_BYTES_PER_ELEMENT 2
#define SCREEN_CHAR_SIZE (SCREEN_ROWS * SCREEN_COLUMNS)
#define SCREEN_ROW_SIZE_BYTES (SCREEN_COLUMNS * SCREEN_BYTES_PER_ELEMENT)
#define SCREEN_SIZE_BYTES (SCREEN_CHAR_SIZE * SCREEN_BYTES_PER_ELEMENT)
#define SCREEN_BUFFER_SIZE_BYTES (SCREEN_SIZE_BYTES * 16)


uint8_t* Screen_VideoBasePointer = (uint8_t*) 0x000B8000;
uint8_t  Screen_Buffer[SCREEN_BUFFER_SIZE_BYTES];
int32_t Screen_BufferReadOffset = 0;
int32_t  Screen_BufferScrollOffset = 25;
/*
    buffer size minus size in lines, minus one screen's worth of lines
*/
uint32_t Screen_BufferScrollOffsetMax;

size_t Screen_CurrentRow = 0;
size_t Screen_CurrentColumn = 0;
uint8_t Screen_Color = 0;

void Screen_Initialize(void)
{
	Screen_CurrentRow = 0;
	Screen_CurrentColumn = 0;
	Screen_BufferScrollOffset = 0;
	Screen_BufferScrollOffsetMax = SCREEN_BUFFER_SIZE_BYTES - (SCREEN_ROWS * SCREEN_ROW_SIZE_BYTES);
	Screen_Color = Screen_VgaEntryColor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	memset(Screen_Buffer,0,SCREEN_BUFFER_SIZE_BYTES);
	Screen_Clear();
}

void Screen_SetColor(uint8_t color)
{
	Screen_Color = color;
}

void Screen_PutEntryAt(unsigned char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = (y * SCREEN_COLUMNS * SCREEN_BYTES_PER_ELEMENT) + (x*SCREEN_BYTES_PER_ELEMENT);
	Screen_Buffer[index] = c;
	Screen_Buffer[index+1] = color;
}

void Screen_PutChar(char c)
{
	unsigned char uc = c;
    if (c == '\n')
    {
        Screen_CurrentRow++;
        Screen_CurrentColumn = 0;
        return;
    }

    if (c == '\t')
    {
        Screen_CurrentColumn += SCREEN_TAB_SIZE;
        return;
    }

	Screen_PutEntryAt(uc, Screen_Color, Screen_CurrentColumn, Screen_CurrentRow);
	if (++Screen_CurrentColumn == SCREEN_COLUMNS)
    {
		Screen_CurrentColumn = 0;
		if (++Screen_CurrentRow == SCREEN_ROWS)
        {
			Screen_CurrentRow = 0;
        }
	}
    
}

void Screen_Write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
    {
		Screen_PutChar(data[i]);
    }
}

void Screen_WriteString(const char* data)
{
	Screen_Write(data, strlen(data));
}

void Screen_Clear()
{
	for (size_t y = 0; y < SCREEN_ROWS; y++)
    {
		for (size_t x = 0; x < SCREEN_COLUMNS; x++)
        {
			const size_t index = y * SCREEN_COLUMNS + x;
			Screen_PutEntryAt(' ',Screen_Color, x,y);
		}
	}
	Screen_Update();
}

void Screen_Update()
{
	memcpy(Screen_VideoBasePointer,&Screen_Buffer[Screen_BufferReadOffset], SCREEN_SIZE_BYTES);
}

uint8_t Screen_VgaEntryColor(enum Screen_VgaColor fg, enum Screen_VgaColor bg)
{
	return fg | bg << 4;
}

uint16_t Screen_VgaEntry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

void Screen_MoveScrollOffset(int32_t offset)
{
	Screen_BufferScrollOffset += offset;
	Screen_BufferReadOffset = SCREEN_ROW_SIZE_BYTES * Screen_BufferScrollOffset - SCREEN_SIZE_BYTES;
	if (Screen_BufferReadOffset < 0)
	{
		Screen_BufferReadOffset = 0;
	}
	else if (Screen_BufferReadOffset > Screen_BufferScrollOffsetMax)
	{
		Screen_BufferReadOffset = Screen_BufferScrollOffsetMax;
	}
	Screen_Update();
}