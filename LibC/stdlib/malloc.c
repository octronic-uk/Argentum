#include <stdlib.h>
#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern Kernel _Kernel;

void* _malloc(uint32_t size, char* file, uint32_t line)
{
    return MemoryDriver_Allocate(&_Kernel.Memory, size, file, line);
}