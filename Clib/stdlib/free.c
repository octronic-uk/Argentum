#include <stdlib.h>
#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern Kernel _Kernel;

void free(void* ptr)
{
    return MemoryDriver_Free(&_Kernel.Memory, ptr);
}