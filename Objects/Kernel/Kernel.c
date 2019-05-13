#include <Objects/Kernel/Kernel.h>
#include <stdio.h>
#include <unistd.h>
#include <Drivers/Memory/Test/MemoryTest.h>
#include <Drivers/Serial/Test/SerialDriverTest.h>
#include <Objects/StorageManager/Test/StorageManagerTest.h>
#include <Objects/GraphicsManager/Test/GraphicsManagerTest.h>

bool Kernel_Constructor(Kernel* self, multiboot_info_t* mbi)
{
	self->MultibootInfo = mbi;
	if(!Kernel_InitDrivers(self))
	{
		printf("Kernel: FATAL - Failed to init drivers\n");
		return false;
	}

	Kernel_TestDrivers(self);

	if (!Kernel_InitObjects(self))
	{
		printf("Kernel: FATAL - Failed to init objects\n");
		return false;
	}

	Kernel_TestObjects(self);

	return true;
}

bool Kernel_InitDrivers(Kernel* self)
{
	if(!TextModeDriver_Constructor(&self->TextMode))
	{
		return false;
	}

	if (!VgaDriver_Constructor(&self->Vga))
	{
		return false;
	}

	VgaDriver_SetScreenMode(&self->Vga, VGA_MODE_TEXT_90_60);
	self->TextMode.Width = 90;
	self->TextMode.Height = 60;

	InterruptDriver_Disable_CLI(&self->Interrupt);
	InterruptDriver_SetMask_PIC1(&self->Interrupt, 0xFF);
	InterruptDriver_SetMask_PIC2(&self->Interrupt, 0xFF);

	if(!MemoryDriver_Constructor(&self->Memory))
	{
		return false;
	}

	if(!InterruptDriver_Constructor(&self->Interrupt))
	{
		return false;
	}

	if (!PITDriver_Constructor(&self->PIT))
	{
		return false;
	}
	if (!SerialDriver_Constructor(&self->Serial))
	{
		return false;
	}

	if(!PS2Driver_Constructor(&self->PS2))
	{
		return false;
	}

	InterruptDriver_SetMask_PIC1(&self->Interrupt, 0x00);
	InterruptDriver_SetMask_PIC2(&self->Interrupt, 0x00);
	InterruptDriver_Enable_STI(&self->Interrupt);

	if (!ACPIDriver_Constructor(&self->ACPI))
	{
		return false;
	}

	if (!PCIDriver_Constructor(&self->PCI))
	{
		return false;
	}

	if (!FloppyDriver_Constructor(&self->Floppy))
	{
		return false;
	}

	if (!ATADriver_Constructor(&self->ATA))
	{
		return false;
	}

	return true;
}

bool Kernel_InitObjects(Kernel* self)
{
	if (!StorageManager_Constructor(&self->StorageManager))
	{
		return false;
	}

	if (!GraphicsManager_Constructor(&self->GraphicsManager))
	{
		return false;
	}
	return true;
}

void Kernel_Destructor(Kernel* self)
{
	Kernel_DestroyObjects(self);
	Kernel_DestroyDrivers(self);
}

void Kernel_DestroyObjects(Kernel* self)
{
	StorageManager_Destructor(&self->StorageManager);
	GraphicsManager_Destructor(&self->GraphicsManager);
}

void Kernel_DestroyDrivers(Kernel* self)
{
	ACPIDriver_Destructor(&self->ACPI);
}

void Kernel_TestDrivers(Kernel* self)
{
	MemoryTest_RunSuite(&self->Memory);
	SerialDriverTest_RunSuite(&self->Serial);
}

void Kernel_TestObjects(Kernel* self)
{
	StorageManagerTest_RunSuite(&self->StorageManager);
	GraphicsManagerTest_RunSuite(&self->GraphicsManager);
}