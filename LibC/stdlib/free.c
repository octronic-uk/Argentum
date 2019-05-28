#include <stdlib.h>
#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern Kernel _Kernel;

void _free(void* ptr, char* file, uint32_t line)
{
    return MemoryDriver_Free(&_Kernel.Memory, ptr, file, line);
}