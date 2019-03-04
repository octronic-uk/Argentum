#include "Screen.h"

static char* VideoBasePointer = (char*)0xb8000;

void tkCoreClearScreen()
{
    /* video memory begins at address 0xb8000 */
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int screensize;

    /* this loops clears the screen
    * there are 25 lines each of 80 columns; each element takes 2 bytes */
    screensize = 80 * 25 * 2;
    while (j < screensize) 
    {
        /* blank character */
        VideoBasePointer[j] = ' ';
        /* attribute-byte */
        VideoBasePointer[j+1] = 0x07;
        j = j + 2;
    }
}

void tkCorePrint(const char* str)
{
    unsigned int j = 0;
    unsigned int i = 0;

    /* this loop writes the string to video memory */
    while (str[j] != '\0') 
    {
        /* the character's ascii */
        VideoBasePointer[i] = str[j];
        /* attribute-byte: give character black bg and light grey fg */
        VideoBasePointer[i+1] = 0x07;
        ++j;
        i = i + 2;
    }
}