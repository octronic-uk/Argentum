#include "multiboot.h"
#include "../Kernel.h"
#include "../Core/Screen/Screen.h"
#include "../Core/Utilities.h"

void some_work_1()
{
    tkScreenPrintLine(" > Hello, World! From Work 1");
}

void some_work_2()
{
    tkScreenPrintLine(" > Hello, World! From Work 2");
}

void kmain()
{
    tkScreenClear();
    tkScreenPrintLine("Taskie Version 1");

    tkKernel k;
    tkKernelConstruct(&k);

    int i;
    tkTask* task1 = tkSchedulerCreateTask(&k.mScheduler, some_work_1);;
    tkTask* task2 = tkSchedulerCreateTask(&k.mScheduler, some_work_2);;
    tkTaskSetName(task1,"T1");
    tkTaskSetName(task2,"T2");

    tkKernelExecute(&k);
    // spin
    while (1) {}
    tkKernelDestruct(&k);
}

