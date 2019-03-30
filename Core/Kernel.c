#include <Kernel.h>

#include <stdio.h>
#include <unistd.h>

#include <Memory/Memory.h>

#include <ACPI/ACPI.h>
#include <Drivers/ATA/ATA.h>
#include <Drivers/Screen/Screen.h>
#include <Drivers/Interrupt/Interrupt.h>
#include <Drivers/PCI/PCI.h>
#include <Drivers/Serial/Serial.h>
#include <Drivers/PS2/Intel8042.h>
#include <Filesystem/FAT/Volume.h>

#include <Scheduler/Scheduler.h>

void Kernel_Constructor
(multiboot_info_t * mbi)
{
	Memory_Constructor(mbi);
	Screen_Initialize();
	Interrupt_Constructor();
	Serial_Constructor();
	Serial_TestPort1();
	printf("Taskie Kernel 1.0\n");
	//printf_to_serial();

	I8042_Constructor();
	
	ACPI_Constructor();
	PCI_Constructor();
	ATA_Constructor();

	Kernel_InitStorageManager();
}

void
Kernel_InitStorageManager()
{
	printf("Kernel: Initialising Storage Manager %s\n",ATA_IDEDevices[0].model);
	FatVolume* part = FatVolume_Constructor(0,0);
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
