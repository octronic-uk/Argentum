#include <Kernel.h>
#include <stdio.h>
#include <unistd.h>

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

	printf("Kernel: BUILD - %s\n", TASKIE_BUILD_DATE);
	return true;
}

bool Kernel_InitDrivers(struct Kernel* self)
{
	printf_SetKernel(self);
	if(!ScreenDriver_Constructor(&self->Screen, self))
	{
		return false;
	}

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

	if (!ACPIDriver_Constructor(&self->ACPI, self))
	{
		return false;
	}

	if (!PCIDriver_Constructor(&self->PCI, self))
	{
		return false;
	}

	if (!ATADriver_Constructor(&self->ATA, self))
	{
		return false;
	}

	printf("Kernel: Init Drivers Complete\n");
	return true;
}

bool Kernel_InitObjects(struct Kernel* self)
{
	if (!StorageManager_Constructor(&self->StorageManager, self))
	{
		return false;
	}
	printf("Kernel: Init Objects Complete\n");
	return true;
}