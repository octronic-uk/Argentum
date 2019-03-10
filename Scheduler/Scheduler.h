#pragma once

#define SCHEDULER_MAX_TASKS 64

#include "../Core/LinkedList/LinkedList.h"
#include "Task.h"


typedef struct
{
    tkLinkedList* mTasks;
} tkScheduler;

tkScheduler* tkSchedulerConstruct();
void tkSchedulerDestruct(tkScheduler* s);

tkTask* tkSchedulerCreateTask(tkScheduler* s, const char* name, void(*fn)(void));
void tkSchedulerExecuteTasks(tkScheduler* s);