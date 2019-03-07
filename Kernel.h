#pragma once
#include "Scheduler/Scheduler.h"

typedef struct 
{
    tkScheduler mScheduler;
} tkKernel;

void tkKernelConstruct(tkKernel* k);
void tkKernelDestruct(tkKernel* k);
void tkKernelExecute(tkKernel* k);
