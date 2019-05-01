#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__)) void abort(void)
{
	printf("kernel: panic: abort()\n");
	while (1) { 
		asm("cli");
		asm("hlt");
	}
	__builtin_unreachable();
}
