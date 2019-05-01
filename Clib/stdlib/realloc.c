#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>

extern struct Kernel _Kernel;

void *realloc(void* ptr, uint32_t size)
{
    return MemoryDriver_Reallocate(&_Kernel.Memory, ptr, size);
}

