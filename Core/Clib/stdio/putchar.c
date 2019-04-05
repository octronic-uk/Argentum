#include <stdio.h>

#include <Kernel.h>
#include <Drivers/Screen/ScreenDriver.h>
#include <Drivers/Serial/SerialDriver.h>

static struct Kernel* _Kernel;
static uint8_t _print_to_serial = 0;

int putchar(int ic)
{
	char c = (char) ic;
	if (_print_to_serial)
	{
		SerialDriver_Write8b(&_Kernel->Serial, &Serial_Port1_8N1, c);
	}
	else
	{
		ScreenDriver_Write(&_Kernel->Screen, &c, sizeof(c));
	}

	return ic;
}

void printf_SetKernel(struct Kernel* kernel)
{
	_Kernel = kernel;
}

void printf_to_serial()
{
	printf("Writing stdout to COM1\n");
	_print_to_serial = 1;
}