#include "TextModeDriver.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Drivers/IO/IODriver.h>

#include "TextModeConstants.h"
#include "TextModeColor.h"



bool TextModeDriver_Constructor(struct TextModeDriver* self)
{
  self->VideoBasePointer = (uint8_t*)TEXTMODE_VBP;
  self->CurrentRow = 0;
  self->CurrentColumn = 0;
  self->EnableEcho = false;
  self->Color = TextModeEntryColor(self, TM_COLOR_LIGHT_GREY, TM_COLOR_BLACK);
  self->Available;
  self->Width = TEXTMODE_COLUMNS_80;
  self->Height = TEXTMODE_ROWS_25;

  TextModeDriver_Clear(self);
  return true;
}

// Text Mode ===================================================================

void TextModeDriver_SetColor(struct TextModeDriver* self, uint8_t color)
{
  self->Color = color;
}

void TextModeDriver_PutEntryAt(struct TextModeDriver* self,uint8_t c, uint8_t color, uint32_t x, uint32_t y)
{
  uint32_t index = (y * self->Width * TEXTMODE_BYTES_PER_ELEMENT) + (x*TEXTMODE_BYTES_PER_ELEMENT);
  self->VideoBasePointer[index] = c;
  self->VideoBasePointer[index+1] = color;

  if (!isspace(c)) TextModeDriver_SetFramebufferCursorPosition(self, (y*self->Width) + x);
}

void TextModeDriver_PutChar(struct TextModeDriver* self, char c)
{
  uint8_t uc = c;
  if (c == '\n')
  {
  while (self->Width - self->CurrentColumn)
  {
    TextModeDriver_PutEntryAt(self, ' ',self->Color,self->CurrentColumn,self->CurrentRow);
    self->CurrentColumn++;
  }
      self->CurrentRow++;
      self->CurrentColumn = 0;
  if (self->CurrentRow >= self->Height)
  {
    TextModeDriver_Scroll(self);
  }
      return;
  }

  if (c == '\t')
  {
    TextModeDriver_PutChar(self, ' ');
    TextModeDriver_PutChar(self, ' ');
    TextModeDriver_PutChar(self, ' ');
    TextModeDriver_PutChar(self, ' ');
    return;
  }

  TextModeDriver_PutEntryAt(self, uc, self->Color, self->CurrentColumn, self->CurrentRow);
  if (++self->CurrentColumn == self->Width)
  {
    self->CurrentColumn = 0;
    if (++self->CurrentRow == self->Height)
    {
      TextModeDriver_Scroll(self);
    }
  }
}

void TextModeDriver_SetHeader(struct TextModeDriver* self, const char* data)
{
  for (uint32_t i = 0; i < self->Width; i++)
  {
    char current = data[i];
    TextModeDriver_PutEntryAt(self, current, self->Color, i, 0);
  }
}

void TextModeDriver_Write(struct TextModeDriver* self, const char* data, uint32_t size)
{
  for (uint32_t i = 0; i < size; i++)
  {
    TextModeDriver_PutChar(self, data[i]);
  }
}

void TextModeDriver_WriteString(struct TextModeDriver* self, const char* data)
{
  TextModeDriver_Write(self, data, strlen(data));
}

void TextModeDriver_ClearRow(struct TextModeDriver* self, uint8_t row)
{
  for (uint32_t x = 0; x < self->Width; x++)
  {
    TextModeDriver_PutEntryAt(self, ' ',self->Color, x, row);
  }
}

void TextModeDriver_Clear(struct TextModeDriver* self)
{
  for (uint32_t y = 0; y < self->Height; y++)
  {
    TextModeDriver_ClearRow(self,y);	
  }
  self->CurrentColumn = 0;
  self->CurrentRow = 0;
}

void TextModeDriver_Scroll(struct TextModeDriver* self)
{
  int row;
  for (row = 0; row < self->Height; row++)
  {
    if (row < self->Height-1)
    {
      TextModeDriver_ClearRow(self,row);
      
    }
    memcpy(
      &self->VideoBasePointer[TEXTMODE_ROW_SIZE_BYTES(self->Width) * row],  
      &self->VideoBasePointer[TEXTMODE_ROW_SIZE_BYTES(self->Width) * (row+1)], 
      TEXTMODE_ROW_SIZE_BYTES(self->Width)
    );
  }
  self->CurrentRow = self->Height-1;
  TextModeDriver_ClearRow(self,row);
  TextModeDriver_SetFramebufferCursorPosition(self, (self->CurrentRow * self->Width));
}

void TextModeDriver_SetCursorPosition(struct TextModeDriver* self, uint8_t column, uint8_t row)
{
  self->CurrentColumn = column;
  self->CurrentRow = row;
}

void TextModeDriver_SetFramebufferCursorPosition(struct TextModeDriver* self, uint16_t position)
{
  IO_WritePort8b(TEXTMODE_FB_IO_CMD,  TEXTMODE_FB_CURSOR_HI);
  IO_WritePort8b(TEXTMODE_FB_IO_DATA, (position >> 8) & 0x00FF);

  IO_WritePort8b(TEXTMODE_FB_IO_CMD,  TEXTMODE_FB_CURSOR_LOW);
  IO_WritePort8b(TEXTMODE_FB_IO_DATA, position & 0x00FF);
}

void TextModeDriver_SetEcho(struct TextModeDriver* self, bool echo)
{
  self->EnableEcho = echo;
}

char TextModeDriver_ReadCharacter(struct TextModeDriver* self)
{

}

uint32_t TextModeDriver_ReadString(struct TextModeDriver* self, char* buffer, uint32_t size)
{

}

// VGA Mode ====================================================================
