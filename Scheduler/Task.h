#pragma once
#include "../Core/Types.h"
#define TASK_NAME_BUFFER_SIZE 64

static unsigned int TaskCurrentID = 0;

typedef struct tkTask
{
    char mName[TASK_NAME_BUFFER_SIZE];
    uint32_t mID;
    struct tkTask* mNext;
    void (*mFunction)(void);
} __attribute__((packed)) tkTask;

tkTask* tkTaskConstruct();
void tkTaskDestruct(tkTask* t);
void tkTaskSetName(tkTask* task, const char* name);
unsigned long tkTaskGetNextID();
