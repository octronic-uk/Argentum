#include <stdio.h>
#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern struct Kernel _Kernel;

__attribute__((__noreturn__)) void abort(void)
{
	printf("PANIC: abort()\n");

	//MemoryDriver_PrintMemoryMap(&_Kernel.Memory);

	while (1) { 
		asm("cli");
		asm("hlt");
	}
	__builtin_unreachable();
}
