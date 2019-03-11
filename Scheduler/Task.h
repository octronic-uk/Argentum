#pragma once
#include "../Core/Types.h"
#define TASK_NAME_BUFFER_SIZE 64

static unsigned int TaskCurrentID = 0;

#define TASK_STATUS_NEW        0
#define TASK_STATUS_WAITING    1
#define TASK_STATUS_RUNNING    2
#define TASK_STATUS_COMPLETED  3
#define TASK_STATUS_DESTROYED  4

typedef struct tkTask
{
    uint8_t mStatus;
    char mName[TASK_NAME_BUFFER_SIZE];
    uint32_t mID;
    struct tkTask* mNext;
    void (*mFunction)(void);
} __attribute__((packed)) tkTask;

tkTask* tkTask_Construct();
void tkTask_Destruct(tkTask* t);
void tkTask_SetName(tkTask* task, const char* name);
uint32_t tkTask_GetNextID();
