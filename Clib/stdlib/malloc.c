#include <stdlib.h>
#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern Kernel _Kernel;

void *malloc(uint32_t size)
{
    return MemoryDriver_Allocate(&_Kernel.Memory, size);
}