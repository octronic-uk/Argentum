#include "Kernel.h"
#include "Memory.h"

struct tkKernel* tkKernelAllocate()
{
   struct tkKernel* kernel = tkMemoryAllocate(sizeof(struct tkKernel));
   return kernel;
}

void tkKernelFree(struct tkKernel* kernel)
{
    tkMemoryFree(sizeof(*kernel));
}

int tkKernelExecute(struct tkKernel* kernel)
{
   return 0;
}
