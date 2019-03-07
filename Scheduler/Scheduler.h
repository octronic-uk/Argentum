#pragma once

#define SCHEDULER_MAX_TASKS 64

#include "../Task/Task.h"

typedef struct
{
    tkTask* mRootTask;
} tkScheduler;

void tkSchedulerConstruct(tkScheduler* s);
void tkSchedulerDestruct(tkScheduler* s);

tkTask* tkSchedulerCreateTask(tkScheduler* s, void(*fn)(void));
void tkSchedulerExecuteTasks(tkScheduler* s);