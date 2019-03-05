#include "Screen.h"

static char* VideoBasePointer = (char*)0xb8000;

void tkScreenClear()
{
    /* this loops clears the screen
    /* video memory begins at address 0xb8000 */
    unsigned int j;
    for (j=0; j < SCREEN_SIZE;) 
    {
        VideoBasePointer[j++] = ' '; /* blank character */
        VideoBasePointer[j++] = 0x07; /* attribute-byte */
    }
}

void tkScreenPrint(const char* str)
{
    /* this loop writes the string to video memory */
    unsigned int j;
    for (j=0; str[j] != '\0'; ++j) 
    {
        if (str[j] == '\n')
        {
            tkScreenNewLine();
        }
        else
        {
            VideoBasePointer[screenCursorLocation++] = str[j]; /* the character's ascii */
            VideoBasePointer[screenCursorLocation++] = 0x07; /* attribute-byte: give character black bg and light grey fg */
        }
    }
}

void tkScreenPrintLine(const char* str)
{
    tkScreenPrint(str);
    tkScreenNewLine();
}

void tkScreenNewLine()
{
	screenCursorLocation = screenCursorLocation + (SCREEN_LINE_SIZE - screenCursorLocation % (SCREEN_LINE_SIZE));
}