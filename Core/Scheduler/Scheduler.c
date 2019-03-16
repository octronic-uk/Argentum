#include "Scheduler.h"
#include "Task.h"

#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

void Scheduler_Constructor()
{
    SchedulerTasks = LinkedList_Constructor();
}

Task* Scheduler_CreateTask(const char* name, void(*e)(void))
{
	#ifdef __DEBUG_SCHEDULER
	printf("Scheduler: Creating a new task");
	#endif
	Task* task = Task_Constructor(name, e);

	#ifdef __DEBUG_SCHEDULER
	printf("Scheduler: Created task at 0x%d\n", addr);
	#endif
	LinkedList_PushBack(SchedulerTasks,task);
	return task;
}

void Scheduler_ExecuteTasks()
{
	#ifdef __DEBUG_SCHEDULER
	    printf("Scheduler: Entering Task Loop\n");
	#endif
	uint32_t numTasks = LinkedList_Size(SchedulerTasks);
	int i;
	for (i=0;i<numTasks;i++)
	{
		LinkedListNode* current = LinkedList_At(SchedulerTasks,i);
        Task* currentTask = (Task*)current->data;

		if (currentTask->mExecutable)
		{
			#ifdef __DEBUG_SCHEDULER
			    printf("Scheduler: Executing Task [%d] %s\n", current->mID, current->getName());
			#endif
			currentTask->mExecutable();
		}
		else
		{
			#ifdef __DEBUG_SCHEDULER
			printf("Scheduler: No function for Task [%d] %s\n", current->mID, current->getName());
			#endif
		}
	}
	#ifdef __DEBUG_SCHEDULER
	printf("Scheduler: Reached end of Task list\n");
	#endif
}
