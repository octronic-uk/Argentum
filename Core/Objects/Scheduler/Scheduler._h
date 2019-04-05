#pragma once

#include <Structures/LinkedList.h>
#include "Task.h"

struct Memory;

struct Scheduler
{
    struct Memory* Memory;
    uint8_t Debug;
    struct LinkedList TaskList;
};

void Scheduler_Constructor(struct Scheduler* self, struct Memory* memory);
void Scheduler_SetDebug(struct Scheduler* self, uint8_t debug);
void Scheduler_CreateTask(struct Scheduler* self, struct Task* task, const char* name, void(*e)(void));
void Scheduler_ExecuteTasks(struct Scheduler* self);
