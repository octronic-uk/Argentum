#include "Scheduler.h"
#include "../Core/Utilities.h"
#include "../Core/Screen/Screen.h"
#include "../Core/Memory/Memory.h"

void tkSchedulerConstruct(tkScheduler* s)
{
   tkScreenPrintLine("Constructing Scheduler");
   s->mRootTask = tkMemoryAllocate(sizeof(tkTask));
   tkTaskConstruct(s->mRootTask);
   tkTaskSetName(s->mRootTask, "Root Task");
}

void tkSchedulerDestruct(tkScheduler* s)
{
   tkScreenPrintLine("Destructing Scheduler");
   tkTaskDestruct(s->mRootTask);
}

tkTask* tkSchedulerCreateTask(tkScheduler* s, void(*fn)(void))
{
   tkScreenPrintLine("Scheduler is creating a new task");
   tkTask* current = s->mRootTask;
   tkTask* last;
   while(current) 
   {
      last = current;
      current = current->mNext;
   }
   current = (tkTask*)tkMemoryAllocate(sizeof(tkTask));
   tkTaskConstruct(current);
   current->mFunction = fn;
   last->mNext = current;
   return current;
}

void tkSchedulerExecuteTasks(tkScheduler* s)
{
   static char buffer[BUFLEN];
   tkTask* current = s->mRootTask;
   while (current)
   {
      memset(buffer,0,sizeof(char)*BUFLEN);
      itoa(current->mID,buffer,BASE_10);

      if (current->mFunction)
      {
         tkScreenPrint("Executing Task [");  tkScreenPrint(buffer);  tkScreenPrint("] "); tkScreenPrintLine(current->mName);
         current->mFunction();
      }
      else
      {
         tkScreenPrint("No function for Task ["); tkScreenPrint(buffer); tkScreenPrint("] "); tkScreenPrintLine(current->mName);
      }
      current = current->mNext;
   }
   tkScreenPrintLine("Reached end of Task list");
}
