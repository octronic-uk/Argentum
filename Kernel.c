#include "Kernel.h"
#include "Core/Memory/Memory.h"
#include "Core/Interrupt/Interrupt.h"
#include "Core/Screen/Screen.h"
#include "Core/Keyboard/Keyboard.h"

void tkKernelInit(tkKernel* k)
{
   tkKeyboardSetupIDT();
   tkInterruptWriteDescriptorTable();
   tkKeyboardIRQInit();
   tkMemoryInitialise();
   k->mScheduler = tkSchedulerConstruct();
}

void tkKernelDestruct(tkKernel* k)
{
   tkSchedulerDestruct(k->mScheduler);
}

void tkKernelExecute(tkKernel* k)
{
   tkSchedulerExecuteTasks(k->mScheduler);
}

tkTask* tkKernelCreateTask(tkKernel* k, const char* name, void(*fn)(void))
{
   return tkSchedulerCreateTask(k->mScheduler, name , fn);
}