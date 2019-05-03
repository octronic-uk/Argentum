#include <stdio.h>

#include <Drivers/PS2/PS2Driver.h>
#include <Objects/Kernel/Kernel.h>

extern struct Kernel _Kernel;

int getchar()
{
    int ch = -1;
    do
    {
        ScancodeParser_WaitForKeyPressAction(&_Kernel.PS2.ScancodeParser);
        ch = ScancodeParser_GetChar(&_Kernel.PS2.ScancodeParser);
        ScancodeParser_ClearCurrentAction(&_Kernel.PS2.ScancodeParser);
    } while (ch < 0);

    printf("%c",ch);
    return ch;
}
