#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;

void* _realloc(void* ptr, uint32_t size, char* file, uint32_t line)
{
    return MemoryDriver_Reallocate(&_Kernel.Memory, ptr, size, file, line);
}

