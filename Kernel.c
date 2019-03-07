#include "Kernel.h"
#include "Core/Memory/Memory.h"
#include "Core/Interrupt/Interrupt.h"
#include "Core/Screen/Screen.h"
#include "Core/Keyboard/Keyboard.h"

void tkKernelConstruct(tkKernel* k)
{
   tkMemoryInitialise();

   tkKeyboardSetupIDT();
   tkInterruptWriteDescriptorTable();
   tkKeyboardIRQInit();
   tkSchedulerConstruct(&k->mScheduler);
}

void tkKernelDestruct(tkKernel* k)
{
}

void tkKernelExecute(tkKernel* k)
{
   tkScreenPrintLine("Entering Task Loop");
   tkSchedulerExecuteTasks(&k->mScheduler);
}