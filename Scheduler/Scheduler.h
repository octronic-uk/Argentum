#pragma once

#define SCHEDULER_MAX_TASKS 64

#include "../DataStructures/LinkedList/LinkedList.h"
#include "Task.h"


typedef struct
{
    tkLinkedList* mTasks;
} tkScheduler;

tkScheduler* tkScheduler_Construct();
void tkScheduler_Destruct(tkScheduler* s);

tkTask* tkScheduler_CreateTask(tkScheduler* s, const char* name, void(*fn)(void));
void tkScheduler_ExecuteTasks(tkScheduler* s);