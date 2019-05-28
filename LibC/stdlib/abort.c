#include <stdio.h>
#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern Kernel _Kernel;

__attribute__((__noreturn__)) void abort_debug(char* file, uint32_t line)
{
	printf("ABORT: %s:%d\n", file, line);

	MemoryDriver_PrintMemoryMap(&_Kernel.Memory);

	while (1) { 
		asm("cli");
		asm("hlt");
	}
	__builtin_unreachable();
}
