#include "Scheduler.h"
#include "../Core/Memory/Memory.h"

struct tkScheduler* tkSchedulerAllocate()
{
    struct tkScheduler* s = tkMemoryAllocate(sizeof(struct tkScheduler));
    return s;
}

void tkSchedulerFree(struct tkScheduler* s)
{
    tkMemoryFree(sizeof(*s));
}

struct tkTask* tkSchedulerCreateTask()
{
   return 0;
}
