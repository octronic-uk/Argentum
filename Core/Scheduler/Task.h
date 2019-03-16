#pragma once
#include <LibC/include/stdint.h>
#define TASK_NAME_LEN 128

typedef enum
{
	TASK_STATUS_NEW = 0,
	TASK_STATUS_WAITING,
	TASK_STATUS_RUNNING,
	TASK_STATUS_COMPLETED,
	TASK_STATUS_DESTROYED
} TaskStatus;


typedef struct
{
	uint32_t mID;
	char mName[TASK_NAME_LEN];
	TaskStatus mStatus;
	void (*mExecutable) (void);
} Task;

static uint32_t CurrentTaskID = 0;

Task* Task_Constructor(const char* name, void(*e)(void));
