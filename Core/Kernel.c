#include "Kernel.h"

#include <stdio.h>

#include <Drivers/ACPI/ACPI.h>
#include <Drivers/ATA/ATA.h>
#include <Drivers/Screen/Screen.h>
#include <Drivers/Memory/Memory.h>
#include <Drivers/Interrupt/Interrupt.h>
#include <Drivers/PCI/PCI.h>
#include <Drivers/Serial/Serial.h>
#include <Drivers/PS2/Intel8042.h>

#include <Scheduler/Scheduler.h>


void Kernel_Constructor
(multiboot_info_t * mbi)
{
   Screen_Initialize();
   printf("Taskie Kernel 1.0\n");
   Memory_Constructor(mbi);
	Interrupt_Constructor(),
	I8042_Constructor();
	Serial_Constructor();
	PCI_Constructor();
	ATA_Constructor();
	Interrupt_WriteDescriptorTable();
	Interrupt_SetMask_PIC1(0x01);
	Interrupt_SetMask_PIC2(0x00);
	Interrupt_Enable_STI();
	Scheduler_Constructor();
	//Serial_SetInterruptEnableRegister(&Serial_Port1_8N1);
	//Serial_TestPort1();
	ACPI_Constructor();
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
