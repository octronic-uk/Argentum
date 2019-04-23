#include <stdlib.h>
#include <Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern struct Kernel _Kernel;

void *malloc(uint32_t size)
{
    return MemoryDriver_Allocate(&_Kernel.Memory, size);
}