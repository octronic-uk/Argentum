#include "Kernel.h"

#include <stdio.h>

#include <Devices/ATA/ATA.h>
#include <Devices/Screen/Screen.h>
#include <Devices/Memory/Memory.h>
#include <Devices/Interrupt/Interrupt.h>
#include <Devices/PCI/PCI.h>
#include <Devices/Keyboard/Keyboard.h>
#include <Devices/Serial/Serial.h>
#include <Devices/PS2/Intel8042.h>

#include <Scheduler/Scheduler.h>


void Kernel_Constructor
(multiboot_info_t * mbi)
{
   Screen_Initialize();
   printf("Taskie Kernel 1.0\n");
   Memory_Constructor(mbi);
	Interrupt_Constructor(),
	Keyboard_Constructor();
	I8042_Constructor();
	//Serial_Constructor();
	//PCI_Constructor();
	//ATA_Constructor();
	Interrupt_WriteDescriptorTable();
	Interrupt_SetMask_PIC1(0x01);
	Interrupt_SetMask_PIC2(0x00);
	Interrupt_Enable_STI();
	//Scheduler_Constructor();
	//Serial_SetInterruptEnableRegister(&Serial_Port1_8N1);
	//Serial_TestPort1();
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
