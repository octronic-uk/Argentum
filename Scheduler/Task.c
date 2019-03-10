#include "Task.h"
#include "../Core/Utilities.h"
#include "../Core/Screen/Screen.h"
#include "../Core/Memory/Memory.h"


tkTask* tkTaskConstruct()
{
    tkTask *t = (tkTask*)tkMemoryAllocate(sizeof(tkTask));
    t->mID = tkTaskGetNextID();
    t->mNext = 0;
    t->mFunction = 0;
    memset(t->mName,0,TASK_NAME_BUFFER_SIZE);
    static char buffer[BUFLEN];
    itoa(t->mID,buffer,BASE_10);
    tkScreenPrint("Task: Constructed "); tkScreenPrintLine(buffer);
    return t;
}

void tkTaskSetName(tkTask* task, const char* name)
{
    unsigned long nameLength = strlen(name);
    // Cap Name Length
    nameLength = nameLength > TASK_NAME_BUFFER_SIZE-1 ? TASK_NAME_BUFFER_SIZE-1 : nameLength;
    memcpy((void*)task->mName, (void*)name, nameLength);
}

void tkTaskDestruct(tkTask* t)
{
    if (t->mNext != 0)
    {
        tkTaskDestruct(t->mNext);
    }
    // Print name of task being destroyed
    static char taskNumBuf[BUFLEN];
    memset(taskNumBuf,0,sizeof(char)*BUFLEN);
    itoa(t->mID,taskNumBuf,BASE_10);
    tkScreenPrint("Task: Destructing ["); tkScreenPrint(taskNumBuf); tkScreenPrint("] "); tkScreenPrintLine(t->mName);
    // Clear Variables
    memset((void*)t->mName, 0, TASK_NAME_BUFFER_SIZE);
    t->mFunction = 0;
    t->mNext = 0;
    t->mID = 0;
    tkMemoryFree(t);
}

unsigned long tkTaskGetNextID()
{
    return TaskCurrentID++;
}