#include "Scheduler.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "Task.h"


void Scheduler_Constructor(struct Scheduler* self, struct Memory* memory)
{
	self->Memory = memory;
	Scheduler_SetDebug(self, true);
	if (self->Debug)
	{
		printf("Scheduler: Constructing\n");
	}
	LinkedList_Constructor(&self->TaskList, self->Memory);
}

void Scheduler_SetDebug(struct Scheduler* self, uint8_t debug)
{
	self->Debug = debug;
}

void Scheduler_CreateTask(struct Scheduler* self, struct Task* task, const char* name, void(*e)(void))
{
	if (self->Debug) 
	{
		printf("Scheduler: Creating a new task");
	}

	Task_Constructor(task, name, e);

	if (self->Debug) 
	{
		printf("Scheduler: Created task at 0x%x\n", task);
	}
	LinkedList_PushBack(&self->TaskList, task);
}

void Scheduler_ExecuteTasks(struct Scheduler* self)
{
	if (self->Debug) 
	{
	    printf("Scheduler: Entering Task Loop\n");
	}
	uint32_t numTasks = LinkedList_Size(&self->TaskList);
	int i;
	for (i=0;i<numTasks;i++)
	{
		struct LinkedListNode* current = LinkedList_At(&self->TaskList,i);
        struct Task* currentTask = (struct Task*)current->Data;

		if (currentTask->Executable)
		{
			if (self->Debug) 
			{
			    printf("Scheduler: Executing Task [%d] %s\n", currentTask->ID, currentTask->Name);
			}
			currentTask->Executable();
		}
		else
		{
			if (self->Debug) 
			{
				printf("Scheduler: No function for Task [%d] %s\n", currentTask->ID, currentTask->Name);
			}
		}
	}
	if (self->Debug) 
	{
		printf("Scheduler: Reached end of Task list\n");
	}
}
