#pragma once
#include "../Task/Task.h"

struct tkScheduler
{
    unsigned int mMaximumTasks;
    struct tkTask* mTaskList;
};

struct tkScheduler* tkSchedulerAllocate();
void tkSchedulerFree(struct tkScheduler* s);

struct tkTask* tkSchedulerCreateTask();
