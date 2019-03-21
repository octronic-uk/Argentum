#include <stdio.h>

#include <Devices/Screen/Screen.h>

int putchar(int ic)
{
	char c = (char) ic;
	Screen_Write(&c, sizeof(c));
	return ic;
}
