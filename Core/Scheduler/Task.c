#include "Task.h"
#include <string.h>
#include <stdio.h>

void Task_Constructor(struct Task* self, const char* name, void(*e)(void))
{
	self->Executable = e;
	self->Status = TASK_STATUS_NEW;
	self->ID = CurrentTaskID++;
    memset(self->Name,0,sizeof(char)*TASK_NAME_LEN);
	if (self->Debug)
	{
		printf("Task: Constructed %s\n", self->Name);
	}
}