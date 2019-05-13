#include <stdio.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Screen/TextMode/TextModeDriver.h>
#include <Drivers/Serial/SerialDriver.h>

extern Kernel _Kernel;

int putchar(int ic)
{
	char c = (char) ic;
	if (&_Kernel.TextMode.Available)
	{
		TextModeDriver_Write(&_Kernel.TextMode, &c, sizeof(c));
	}
	if (&_Kernel.Serial.Initialised)
	{
		SerialDriver_Write8b(&_Kernel.Serial, &SerialPort1_8N1, c);
	}
	return ic;
}