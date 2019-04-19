#include <Kernel.h>
#include <stdio.h>
#include <unistd.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>

bool Kernel_Constructor(struct Kernel* self, multiboot_info_t* mbi)
{
	self->MultibootInfo = mbi;
	if(!Kernel_InitDrivers(self))
	{
		printf("Kernel: FATAL - Failed to init drivers\n");
		return false;
	}

	if (!Kernel_InitObjects(self))
	{
		printf("Kernel: FATAL - Failed to init objects\n");
		return false;
	}

	Kernel_TestStorageManager(self);

	return true;
}

bool Kernel_InitDrivers(struct Kernel* self)
{
	printf_SetKernel(self);
	if(!ScreenDriver_Constructor(&self->Screen, self))
	{
		return false;
	}
	ScreenDriver_SetCursorPosition(&self->Screen, 0,2);

	InterruptDriver_SetMask_PIC1(&self->Interrupt, 0xFF);
	InterruptDriver_SetMask_PIC2(&self->Interrupt, 0xFF);

	if(!MemoryDriver_Constructor(&self->Memory, self))
	{
		return false;
	}

	if(!InterruptDriver_Constructor(&self->Interrupt, self))
	{
		return false;
	}

	if (!PITDriver_Constructor(&self->PIT, self))
	{
		return false;
	}
	if (!SerialDriver_Constructor(&self->Serial, self))
	{
		return false;
	}
	//SerialDriver_TestPort1(&self->Serial);
	if(!PS2Driver_Constructor(&self->PS2, self))
	{
		return false;
	}

	InterruptDriver_SetMask_PIC1(&self->Interrupt, 0x00);
	InterruptDriver_SetMask_PIC2(&self->Interrupt, 0x00);
	InterruptDriver_Enable_STI(&self->Interrupt);

/*
	if (!ACPIDriver_Constructor(&self->ACPI, self))
	{
		return false;
	}
*/

	if (!PCIDriver_Constructor(&self->PCI, self))
	{
		return false;
	}

	if (!FloppyDriver_Constructor(&self->Floppy,self))
	{
		return false;
	}

	if (!ATADriver_Constructor(&self->ATA, self))
	{
		return false;
	}

	PS2Driver_WaitForKeyPress("Drivers init complete");
	return true;
}

bool Kernel_InitObjects(struct Kernel* self)
{
	if (!StorageManager_Constructor(&self->StorageManager, self))
	{
		return false;
	}
	PS2Driver_WaitForKeyPress("Object init complete");
	return true;
}

bool Kernel_TestStorageManager(struct Kernel* self)
{
	printf("Kernel: Testing StorageManager\n");
	struct StorageManager* sm = &self->StorageManager;
	StorageManager_ListDrives(sm);
	struct SMDrive* drive0 = StorageManager_GetDrive(sm, 0);

	if (drive0)
	{
		printf("Kernel: Got Drive 0 from StorageManager\n");
		SMDrive_ListVolumes(drive0);
		struct SMVolume* drv0vol0 = SMDrive_GetVolume(drive0,0);
		if (drv0vol0)
		{
			printf("Kernel: Got drive0/volume0 \n");
			PS2Driver_WaitForKeyPress("Kernel Pause");
		}
		else
		{
			printf("Kernel: Error getting drive0/volume0");
			PS2Driver_WaitForKeyPress("Kernel Pause");
		}
		
	}

	//SMPath_TestParser();

	//char* file_path = "/0/0/STL_FILE/SquirrelChess.rar";
	char* file_path = "/0/0/test.txt";

	struct SMDirectoryEntry file;

	if(!StorageManager_Open(&self->StorageManager, &file, file_path))
	{
		printf("Kernel: StorageManager has FAILED to open the file at %s\n", file_path);
		PS2Driver_WaitForKeyPress("Kernel Pause");
		return false;
	}

	if (!SMDirectoryEntry_IsFile(&file))
	{
		printf("Kernel: Directory %s is not a file\n", file_path);
		PS2Driver_WaitForKeyPress("Kernel Pause");
	}

	printf("Kernel: StorageManager has opened the file at %s\n", file_path);
	PS2Driver_WaitForKeyPress("Kernel Pause");
	
	return false;
}