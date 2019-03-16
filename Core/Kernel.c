#include "Kernel.h"

#include <Devices/Screen/tty.h>
#include <Devices/Memory/Memory.h>
#include <Devices/Interrupt/Interrupt.h>
#include <Devices/PCI/PCI.h>
#include <Devices/Keyboard/Keyboard.h>
#include <Scheduler/Scheduler.h>
#include <LibC/include/stdio.h>

void Kernel_Constructor
(multiboot_info_t * mbi)
{
    terminal_initialize();
    printf("Taskie Kernel 1.0\n");
    Memory_Constructor(mbi);
	Interrupt_Constructor(),
	Scheduler_Constructor(),
	PCI_Constructor();
	Keyboard_Constructor();
	Interrupt_WriteDescriptorTable();
	Keyboard_IRQInit();
}

void
Kernel_ExecuteTasks
()
{
   Scheduler_ExecuteTasks();
}

Task*
Kernel_CreateTask
(const char* name, void(*e)(void))
{
   return Scheduler_CreateTask(name,e);
}
