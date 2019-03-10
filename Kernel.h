#pragma once
#include "Scheduler/Scheduler.h"

typedef struct 
{
    tkScheduler* mScheduler;
} tkKernel;

void tkKernelInit(tkKernel* k);
void tkKernelDestruct(tkKernel* k);
void tkKernelExecute(tkKernel* k);
tkTask* tkKernelCreateTask(tkKernel* k, const char* name, void(*fn)(void));