#include <stdlib.h>
#include <Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern struct Kernel _Kernel;

void free(void* ptr)
{
    return MemoryDriver_Free(&_Kernel.Memory, ptr);
}