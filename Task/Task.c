#include "Task.h"
#include "../Core/Utilities.h"
#include "../Core/Screen/Screen.h"


void tkTaskConstruct(tkTask* t)
{
    t->mID = tkTaskGetNextID();
    t->mNext = 0;
    t->mFunction = 0;
    static char buffer[BUFLEN];
    memset(buffer,0,sizeof(char)*BUFLEN);
    itoa(t->mID,buffer,BASE_10);
    tkScreenPrint("Creating Task "); tkScreenPrintLine(buffer);
}

void tkTaskSetName(tkTask* task, const char* name)
{
    unsigned long nameLength = strlen(name);
    // Cap Name Length
    nameLength = nameLength > TASK_NAME_BUFFER_SIZE-1 ?
                 TASK_NAME_BUFFER_SIZE-1 : nameLength;
    memcpy((void*)task->mName, (void*)name, nameLength);
}

void tkTaskDestruct(tkTask* t)
{

}

unsigned long tkTaskGetNextID()
{
    return TaskCurrentID++;
}