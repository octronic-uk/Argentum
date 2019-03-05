#include "Kernel.h"
#include "Core/Memory/Memory.h"
#include "Core/Interrupt/Interrupt.h"
#include "Core/Screen/Screen.h"
#include "Core/Keyboard/Keyboard.h"

struct tkKernel* tkKernelAllocate()
{
   tkScreenPrintLine("Allocating Kernel Memory");
   struct tkKernel* kernel = tkMemoryAllocate(sizeof(struct tkKernel));
   return kernel;
}

void tkKernelFree(struct tkKernel* kernel)
{
   tkScreenPrintLine("Freeing Kernel Memory");
    tkMemoryFree(sizeof(*kernel));
}

int tkKernelExecute(struct tkKernel* kernel)
{
   tkKeyboardSetupIDT();
   tkInterruptInitialiseDescriptorTable();
   tkKeyboardIRQInit();
   tkScreenPrintLine("Entering Task Loop");
   while(1)
   {

   }
   return 0;
}
