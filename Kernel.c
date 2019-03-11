#include "Kernel.h"
#include "Core/Memory/Memory.h"
#include "Core/Interrupt/Interrupt.h"
#include "Core/Screen/Screen.h"
#include "Core/Keyboard/Keyboard.h"
#include "Core/PCI/PCI.h"

void tkKernel_Init(tkKernel* k)
{
   tkPCI_CheckAllBuses();
   tkKeyboard_SetupIDT();
   tkInterrupt_WriteDescriptorTable();
   tkKeyboard_IRQInit();
   tkMemory_Initialise();
   k->mScheduler = tkScheduler_Construct();
}

void tkKernel_Destruct(tkKernel* k)
{
   tkScheduler_Destruct(k->mScheduler);
}

void tkKernel_Execute(tkKernel* k)
{
   tkScheduler_ExecuteTasks(k->mScheduler);
}

tkTask* tkKernel_CreateTask(tkKernel* k, const char* name, void(*fn)(void))
{
   return tkScheduler_CreateTask(k->mScheduler, name , fn);
}