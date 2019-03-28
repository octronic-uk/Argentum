#include "Task.h"
#include <string.h>
#include <Memory/Memory.h>

Task* Task_Constructor(const char* name, void(*e)(void))
{
	Task* task = Memory_Allocate(sizeof(Task));
	task->mExecutable = e;
	task->mStatus = TASK_STATUS_NEW;
	task->mID = CurrentTaskID++;
    memset(task->mName,0,sizeof(char)*TASK_NAME_LEN);
	#ifdef __DEBUG_TASK
		printf("Task: Constructed %s\n", mName);
	#endif
}

void Task_Destructor(Task* task)
{
	// Print name of task being destroyed
	#ifdef __DEBUG_TASK
		printf("Destroying task %s\n", mName);
	#endif
	task->mStatus = TASK_STATUS_DESTROYED;
	Memory_Free(task);
}
