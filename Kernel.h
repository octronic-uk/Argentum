#pragma once
#include "Scheduler/Scheduler.h"

typedef struct 
{
    tkScheduler* mScheduler;
} tkKernel;

void tkKernel_Init(tkKernel* k);
void tkKernel_Destruct(tkKernel* k);
void tkKernel_Execute(tkKernel* k);
tkTask* tkKernel_CreateTask(tkKernel* k, const char* name, void(*fn)(void));