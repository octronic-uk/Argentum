#include <stdio.h>

#include <Kernel.h>
#include <Drivers/Screen/Screen.h>
#include <Drivers/Serial/Serial.h>

extern struct Kernel kernel;

uint8_t _print_to_serial = 0;

int putchar(int ic)
{
	char c = (char) ic;
	if (_print_to_serial)
	{
		Serial_Write8b(&kernel.Serial, &Serial_Port1_8N1, c);
	}
	else
	{
		Screen_Write(&kernel.Screen, &c, sizeof(c));
	}

	return ic;
}

void printf_to_serial()
{
	printf("Writing stdout to COM1\n");
	_print_to_serial = 1;
}