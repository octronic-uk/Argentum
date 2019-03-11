#include "Screen.h"
#include "../Utilities.h"

void tkScreen_Init()
{
    memset(ScreenBuffer,0,sizeof(char)*SCREEN_BUFFER_SIZE_BYTES);
    memset(ScreenVideoBasePointer,0,sizeof(char)*SCREEN_SIZE_BYTES);
    ScreenScrollOffsetMax = (SCREEN_BUFFER_SIZE_BYTES - SCREEN_SIZE_BYTES) / SCREEN_ROW_SIZE_BYTES;
    tkScreen_Update();
}

void tkScreen_Clear()
{
    uint32_t j;
    for (j=0; j < SCREEN_CHAR_SIZE; j++) 
    {
        ScreenBuffer[ScreenBufferWriteOffset++] = ' ';
        ScreenBuffer[ScreenBufferWriteOffset++] = ScreenCharAttributes;
    }
    tkScreen_Update();
}

void tkScreen_Update()
{
    memcpy(ScreenVideoBasePointer,&ScreenBuffer[ScreenBufferReadOffset],sizeof(char)*SCREEN_SIZE_BYTES);
}

void tkScreen_Print(const char* str)
{
    uint32_t i;
    for (i=0; str[i] != '\0'; i++) 
    {
        if (str[i] == '\n')
        {
            tkScreen_NewLine();
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
    tkScreen_Update();
}

void tkScreen_PrintLine(const char* str)
{
    tkScreen_Print(str);
    tkScreen_NewLine();
}

void tkScreen_NewLine()
{
    int32_t screenCurrentColumn = ScreenBufferWriteOffset % SCREEN_ROW_SIZE_BYTES;
    ScreenBufferWriteOffset = ScreenBufferWriteOffset + SCREEN_ROW_SIZE_BYTES - screenCurrentColumn; 
    tkScreen_Update();
}

void tkScreen_MoveScrollOffset(int32_t offset)
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
    tkScreen_Update();
}

void tkScreen_SetCharAttributes(uint8_t attributes)
{
    ScreenCharAttributes = attributes;
}