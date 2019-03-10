#include "Screen.h"
#include "../Utilities.h"

void tkScreenInit()
{
    memset(ScreenBuffer,0,sizeof(char)*SCREEN_BUFFER_SIZE_BYTES);
    memset(ScreenVideoBasePointer,0,sizeof(char)*SCREEN_SIZE_BYTES);
    ScreenScrollOffsetMax = (SCREEN_BUFFER_SIZE_BYTES - SCREEN_SIZE_BYTES) / SCREEN_ROW_SIZE_BYTES;
    tkScreenUpdate();
}

void tkScreenClear()
{
    uint32_t j;
    for (j=0; j < SCREEN_CHAR_SIZE; j++) 
    {
        ScreenBuffer[ScreenBufferWriteOffset++] = ' ';
        ScreenBuffer[ScreenBufferWriteOffset++] = ScreenCharAttributes;
    }
    tkScreenUpdate();
}

void tkScreenUpdate()
{
    memcpy(ScreenVideoBasePointer,&ScreenBuffer[ScreenBufferReadOffset],sizeof(char)*SCREEN_SIZE_BYTES);
}

void tkScreenPrint(const char* str)
{
    uint32_t i;
    for (i=0; str[i] != '\0'; i++) 
    {
        if (str[i] == '\n')
        {
            tkScreenNewLine();
        }
        else if (str[i] == '\t')
        {
            int j;
            for (j=0;j<4;j++)
            {
                ScreenBuffer[ScreenBufferWriteOffset++] = ' ';
                ScreenBuffer[ScreenBufferWriteOffset++] = ScreenCharAttributes;
            }

        }
        else
        {
            ScreenBuffer[ScreenBufferWriteOffset++] = str[i];
            ScreenBuffer[ScreenBufferWriteOffset++] = ScreenCharAttributes;
        }
    }
    tkScreenUpdate();
}

void tkScreenPrintLine(const char* str)
{
    tkScreenPrint(str);
    tkScreenNewLine();
}

void tkScreenNewLine()
{
    int32_t screenCurrentColumn = ScreenBufferWriteOffset % SCREEN_ROW_SIZE_BYTES;
    ScreenBufferWriteOffset = ScreenBufferWriteOffset + SCREEN_ROW_SIZE_BYTES - screenCurrentColumn; 
    tkScreenUpdate();
}

void tkScreenMoveScrollOffset(int32_t offset)
{
    ScreenScrollOffset += offset;
    if (ScreenScrollOffset < 0) 
    {
        ScreenScrollOffset = 0;
    }
    if (ScreenScrollOffset > ScreenScrollOffsetMax)
    {
        ScreenScrollOffset = ScreenScrollOffsetMax;
    }
    ScreenBufferReadOffset = ScreenScrollOffset * SCREEN_ROW_SIZE_BYTES;
    tkScreenUpdate();
}

void tkScreenSetCharAttributes(uint8_t attributes)
{
    ScreenCharAttributes = attributes;
}