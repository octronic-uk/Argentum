#include "Scheduler.h"
#include "Task.h"

#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

uint8_t Scheduler_Debug = 0;
static LinkedList* SchedulerTasks = 0;

void Scheduler_Constructor()
{
	Scheduler_SetDebug(1);
	if (Scheduler_Debug)
	{
		printf("Scheduler: Constructing\n");
	}
    SchedulerTasks = LinkedList_Constructor();
}

void Scheduler_SetDebug(uint8_t debug)
{
	Scheduler_Debug = debug;
}

Task* Scheduler_CreateTask(const char* name, void(*e)(void))
{
	if (Scheduler_Debug) {
		printf("Scheduler: Creating a new task");
	}
	Task* task = Task_Constructor(name, e);

	if (Scheduler_Debug) {
		printf("Scheduler: Created task at 0x%x\n", task);
	}
	LinkedList_PushBack(SchedulerTasks,task);
	return task;
}

void Scheduler_ExecuteTasks()
{
	if (Scheduler_Debug) {
	    printf("Scheduler: Entering Task Loop\n");
	}
	uint32_t numTasks = LinkedList_Size(SchedulerTasks);
	int i;
	for (i=0;i<numTasks;i++)
	{
		LinkedListNode* current = LinkedList_At(SchedulerTasks,i);
        Task* currentTask = (Task*)current->data;

		if (currentTask->mExecutable)
		{
			if (Scheduler_Debug) {
			    printf("Scheduler: Executing Task [%d] %s\n", currentTask->mID, currentTask->mName);
			}
			currentTask->mExecutable();
		}
		else
		{
			if (Scheduler_Debug) {
				printf("Scheduler: No function for Task [%d] %s\n", currentTask->mID, currentTask->mName);
			}
		}
	}
	if (Scheduler_Debug) {
		printf("Scheduler: Reached end of Task list\n");
	}
}
