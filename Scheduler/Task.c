#include "Task.h"
#include "../Core/Utilities.h"
#include "../Core/Screen/Screen.h"
#include "../Core/Memory/Memory.h"


tkTask* tkTask_Construct()
{
    tkTask *t = (tkTask*)tkMemory_Allocate(sizeof(tkTask));
    t->mStatus = TASK_STATUS_NEW;
    t->mID = tkTask_GetNextID();
    t->mNext = 0;
    t->mFunction = 0;
    memset(t->mName,0,TASK_NAME_BUFFER_SIZE);
    #ifdef __DEBUG_TASK
        static char buffer[BUFLEN];
        itoa(t->mID,buffer,BASE_10);
        tkScreen_Print("Task: Constructed "); 
        tkScreen_PrintLine(buffer);
    #endif
    return t;
}

void tkTask_SetName(tkTask* task, const char* name)
{
    uint32_t nameLength = strlen(name);
    // Cap Name Length
    nameLength = nameLength > TASK_NAME_BUFFER_SIZE-1 ? TASK_NAME_BUFFER_SIZE-1 : nameLength;
    memcpy((void*)task->mName, (void*)name, nameLength);
}

void tkTask_Destruct(tkTask* t)
{
    if (t->mNext != 0)
    {
        tkTask_Destruct(t->mNext);
    }
    // Print name of task being destroyed
    #ifdef __DEBUG_TASK
        static char taskNumBuf[BUFLEN];
        memset(taskNumBuf,0,sizeof(char)*BUFLEN);
        itoa(t->mID,taskNumBuf,BASE_10);
        tkScreen_Print("Task: Destructing ["); 
        tkScreen_Print(taskNumBuf); 
        tkScreen_Print("] "); 
        tkScreen_PrintLine(t->mName);
    #endif
    // Clear Variables
    memset((void*)t->mName, 0, TASK_NAME_BUFFER_SIZE);
    t->mStatus = TASK_STATUS_DESTROYED;
    t->mFunction = 0;
    t->mNext = 0;
    t->mID = 0;
    tkMemory_Free(t);
}

uint32_t tkTask_GetNextID()
{
    return TaskCurrentID++;
}