#include <Kernel.h>

#include <stdio.h>

#include <Memory/Memory.h>

#include <ACPI/ACPI.h>
#include <Drivers/ATA/ATA.h>
#include <Drivers/Screen/Screen.h>
#include <Drivers/Interrupt/Interrupt.h>
#include <Drivers/PCI/PCI.h>
#include <Drivers/Serial/Serial.h>
#include <Drivers/PS2/Intel8042.h>
#include <Filesystem/MBR/MBR.h>

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
	print_to_serial();

	I8042_Constructor();
	
	ACPI_Constructor();
	PCI_Constructor();
	ATA_Constructor();

	Kernel_SetupDrive();
	//Scheduler_Constructor();
}

void
Kernel_SetupDrive()
{
	printf("Kernel: Setting up drive 0 %s\n",ATA_IDEDevices[0].model);
	uint8_t lba48 = 0;
	if (ATA_IDEDevices[0].reserved == 1)
	{
		if(ATA_DeviceUsesLBA48(ATA_IDEDevices[0]))
		{
			printf("Kernel: IDE Device 0 is LBA48\n");
			lba48 = 1;
		}
		else
		{
			printf("Kernel: IDE Device 0 is CHS/LBA24\n");
		}
	}
	else
	{
		printf("Kernel: No Device 0\n");
		return;
	}

	if (lba48)
	{
		uint8_t data[512];
		ATA_IDEAccess(0,0,0,1,0,(uint32_t)&data[0]);
		printf("Kernel: Got ATA Data...\n");
		int i;
		/*
		for (i=0; i<512; i+=8)
		{
			printf("%x %x %x %x %x %x %x %x\n",
				data[i],   data[i+1], data[i+2], data[i+3],
				data[i+4], data[i+5], data[i+6], data[i+7]
			);
		}
		*/
		MBR_Record* record = (MBR_Record*)&data[0];
		MBR_DebugRecord(record);
	}
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
