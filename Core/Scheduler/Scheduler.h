#pragma once
#include <Structures/LinkedList.h>
#include "Task.h"


void Scheduler_SetDebug(uint8_t debug);
void Scheduler_Constructor();
Task* Scheduler_CreateTask(const char* name, void(*e)(void));
void Scheduler_ExecuteTasks();
