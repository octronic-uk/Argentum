#pragma once
#include <Structures/LinkedList.h>
#include "Task.h"

static LinkedList* SchedulerTasks;

void Scheduler_Constructor();
Task* Scheduler_CreateTask(const char* name, void(*e)(void));
void Scheduler_ExecuteTasks();
