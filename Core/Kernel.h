#pragma once
#include "../Boot/multiboot.h"
#include <Scheduler/Task.h>


void Kernel_Constructor(multiboot_info_t* mbi);
void Kernel_ExecuteTasks();
Task* Kernel_CreateTask(const char* name, void(*e)(void));
void Kernel_InitStorageManager();