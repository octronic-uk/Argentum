#include "Scheduler.h"
#include "../Core/Utilities.h"
#include "../Core/Screen/Screen.h"
#include "../Core/Memory/Memory.h"

tkScheduler* tkScheduler_Construct()
{
   #ifdef __DEBUG_SCHEDULER
      tkScreen_PrintLine("Scheduler: Constructing");
   #endif
   tkScheduler* s = (tkScheduler*)tkMemory_Allocate(sizeof(tkScheduler));
   s->mTasks = tkLinkedList_Construct();
   return s;
}

void tkScheduler_Destruct(tkScheduler* s)
{
   #ifdef __DEBUG_SCHEDULER
      tkScreen_PrintLine("Scheduler: Destructing");
   #endif
   tkLinkedList_FreeAllData(s->mTasks);
   tkLinkedList_Destruct(s->mTasks);
   tkMemory_Free(s);
}

tkTask* tkScheduler_CreateTask(tkScheduler* s, const char* name, void(*fn)(void))
{
   #ifdef __DEBUG_SCHEDULER
      tkScreen_PrintLine("Scheduler: Creating a new task");
   #endif
   tkTask* task = (tkTask*)tkTask_Construct();
   task->mFunction = fn;
   tkTask_SetName(task,name);
   #ifdef __DEBUG_SCHEDULER
      static char addr[BUFLEN];
      memset(addr,0,BUFLEN);
      itoa((uint32_t)task,addr,BASE_16);
      tkScreen_Print("Scheduler: Created task at 0x"); tkScreen_PrintLine(addr);
   #endif
   tkLinkedList_Insert(s->mTasks,task);
   return task;
}

void tkScheduler_ExecuteTasks(tkScheduler* s)
{
   #ifdef __DEBUG_SCHEDULER
   tkScreen_PrintLine("Scheduler: Entering Task Loop");
   static char buffer[BUFLEN];
   #endif
   uint32_t numTasks = tkLinkedList_Size(s->mTasks);
   int i;
   for (i=0;i<numTasks;i++)
   {
      tkLinkedListItem* currentItem = tkLinkedList_At(s->mTasks,i);
      tkTask* current = currentItem->mData;

      #ifdef __DEBUG_SCHEDULER
      memset(buffer,0,sizeof(char)*BUFLEN);
      itoa(current->mID,buffer,BASE_10);
      #endif

      if (current->mFunction)
      {
         #ifdef __DEBUG_SCHEDULER
            tkScreen_Print("Scheduler: Executing Task [");  
            tkScreen_Print(buffer);  
            tkScreen_Print("] "); 
            tkScreen_PrintLine(current->mName);
         #endif
         current->mFunction();
      }
      else
      {
         #ifdef __DEBUG_SCHEDULER
            tkScreen_Print("Scheduler: No function for Task ["); 
            tkScreen_Print(buffer); 
            tkScreen_Print("] "); 
            tkScreen_PrintLine(current->mName);
         #endif
      }
   }
   #ifdef __DEBUG_SCHEDULER
      tkScreen_PrintLine("Scheduler: Reached end of Task list");
   #endif
}
