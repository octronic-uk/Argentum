#include "Scheduler.h"
#include "../Core/Utilities.h"
#include "../Core/Screen/Screen.h"
#include "../Core/Memory/Memory.h"

tkScheduler* tkSchedulerConstruct()
{
   tkScreenPrintLine("Scheduler: Constructing");
   tkScheduler* s = (tkScheduler*)tkMemoryAllocate(sizeof(tkScheduler));
   s->mTasks = tkLinkedListConstruct();
   return s;
}

void tkSchedulerDestruct(tkScheduler* s)
{
   tkScreenPrintLine("Scheduler: Destructing");
   tkLinkedListFreeAllData(s->mTasks);
   tkLinkedListDestruct(s->mTasks);
   tkMemoryFree(s);
}

tkTask* tkSchedulerCreateTask(tkScheduler* s, const char* name, void(*fn)(void))
{
   tkScreenPrintLine("Scheduler: Creating a new task");
   tkTask* task = (tkTask*)tkTaskConstruct();
   task->mFunction = fn;
   tkTaskSetName(task,name);
   static char addr[BUFLEN];
   memset(addr,0,BUFLEN);
   itoa((uint32_t)task,addr,BASE_16);
   tkScreenPrint("Scheduler: Created task at 0x"); tkScreenPrintLine(addr);
   tkLinkedListInsert(s->mTasks,task);
   return task;
}

void tkSchedulerExecuteTasks(tkScheduler* s)
{
   tkScreenPrintLine("Scheduler: Entering Task Loop");
   static char buffer[BUFLEN];
   uint32_t numTasks = tkLinkedListSize(s->mTasks);
   int i;
   for (i=0;i<numTasks;i++)
   {
      tkLinkedListItem* currentItem = tkLinkedListAt(s->mTasks,i);
      tkTask* current = currentItem->mData;
      memset(buffer,0,sizeof(char)*BUFLEN);
      itoa(current->mID,buffer,BASE_10);

      if (current->mFunction)
      {
         tkScreenPrint("Scheduler: Executing Task [");  
         tkScreenPrint(buffer);  
         tkScreenPrint("] "); 
         tkScreenPrintLine(current->mName);
         current->mFunction();
      }
      else
      {
         tkScreenPrint("Scheduler: No function for Task ["); tkScreenPrint(buffer); tkScreenPrint("] "); tkScreenPrintLine(current->mName);
      }
   }
   tkScreenPrintLine("Scheduler: Reached end of Task list");
}
