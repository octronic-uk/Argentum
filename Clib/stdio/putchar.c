#include <stdio.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Screen/ScreenDriver.h>
#include <Drivers/Serial/SerialDriver.h>

extern struct Kernel _Kernel;

int putchar(int ic)
{
	char c = (char) ic;
	ScreenDriver_Write(&_Kernel.Screen, &c, sizeof(c));

	return ic;
}