#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TASK_NAME_LEN 128

enum TaskStatus
{
	TASK_STATUS_NEW = 0,
	TASK_STATUS_WAITING,
	TASK_STATUS_RUNNING,
	TASK_STATUS_COMPLETED,
	TASK_STATUS_DESTROYED
};

static uint32_t CurrentTaskID = 0;

struct Task
{
	bool Debug;
	uint32_t ID;
	char Name[TASK_NAME_LEN];
	enum TaskStatus Status;
	void (*Executable) (void);
};

void Task_Constructor(struct Task* self, const char* name, void(*e)(void));