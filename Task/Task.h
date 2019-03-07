#pragma once

#define TASK_NAME_BUFFER_SIZE 64

static unsigned int TaskCurrentID = 0;

typedef struct tkTask
{
    const char mName[TASK_NAME_BUFFER_SIZE];
    unsigned long mID;
    struct tkTask* mNext;
    void (*mFunction)(void);
}  tkTask;

void tkTaskConstruct(tkTask* t);
void tkTaskDestruct(tkTask* t);
void tkTaskSetName(tkTask* task, const char* name);
unsigned long tkTaskGetNextID();
