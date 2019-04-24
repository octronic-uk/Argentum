#include "FPUDriver.h"

void FPUDriver_LoadControlWord(const uint16_t control)
{
    asm volatile("fldcw %0;"::"m"(control)); 
}