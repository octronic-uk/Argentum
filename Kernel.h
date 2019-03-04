#pragma once
#include "Scheduler/Scheduler.h"

struct tkKernel
{
    struct tkScheduler *mScheduler;
};

struct tkKernel* tkKernelAllocate();
void tkKernelFree(struct tkKernel* kernel);

int tkKernelExecute(struct tkKernel* kernel);
