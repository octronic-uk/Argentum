#include <stdio.h>

#include <Drivers/Screen/Screen.h>

int putchar(int ic)
{
	char c = (char) ic;
	Screen_Write(&c, sizeof(c));
	return ic;
}
