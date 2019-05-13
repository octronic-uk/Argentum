#include <stdio.h>

#include <Drivers/PS2/ScancodeParser.h>
#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;

int getchar()
{
    int ch = -1;
    while (ch < 0)
    {
        ScancodeParser_WaitForKeyPressAction(&_Kernel.PS2.ScancodeParser);
        ch = ScancodeParser_GetChar(&_Kernel.PS2.ScancodeParser);
        ScancodeParser_ClearCurrentAction(&_Kernel.PS2.ScancodeParser);
    }

    printf("%c",ch);
    return ch;
}
