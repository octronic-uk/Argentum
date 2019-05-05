#include <stdio.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Screen/ScreenDriver.h>
#include <Drivers/Serial/SerialDriver.h>

extern struct Kernel _Kernel;

int putchar(int ic)
{
	char c = (char) ic;
	ScreenDriver_Write(&_Kernel.Screen, &c, sizeof(c));
	if (&_Kernel.Serial.Initialised)
	{
		SerialDriver_Write8b(&_Kernel.Serial, &Serial_Port1_8N1, c);
	}
	return ic;
}