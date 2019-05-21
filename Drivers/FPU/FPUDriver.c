#include "FPUDriver.h"
#include <stdio.h>
#include <stddef.h>

/**
 * Set the FPU control word
 *
 * @param cw What to set the control word to.
 */
void FPUDriver_SetControlWord(const uint16_t cw) 
{
	asm volatile("fldcw %0" :: "m"(cw));
}

/**
 * Enable the FPU
 *
 * We are assuming that we have one to begin with, but since we
 * only really operate on 686 machines, we do, so we're not
 * going to bother checking.
 */
void FPUDriver_EnableFPU() 
{
	printf("FPU: Enabling floating-point arithmetic unit\n");
	size_t cr4;
	asm volatile ("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= 0x200;
	asm volatile ("mov %0, %%cr4" :: "r"(cr4));
	FPUDriver_SetControlWord(0x37F);
}