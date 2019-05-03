#include <Objects/Kernel/Kernel.h>
#include <stdio.h>
#include <unistd.h>
#include <Drivers/Memory/MemoryTest.h>

bool Kernel_Constructor(struct Kernel* self, multiboot_info_t* mbi)
{
	self->MultibootInfo = mbi;
	if(!Kernel_InitDrivers(self))
	{
		printf("Kernel: FATAL - Failed to init drivers\n");
		return false;
	}

	//MemoryTest_RunSuite(&self->Memory);
	//SerialDriver_TestPort1(&self->Serial);

	if (!Kernel_InitObjects(self))
	{
		printf("Kernel: FATAL - Failed to init objects\n");
		return false;
	}

	//StorageManager_Test(&self->StorageManager);

	return true;
}

bool Kernel_InitDrivers(struct Kernel* self)
{
	if(!ScreenDriver_Constructor(&self->Screen))
	{
		return false;
	}

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

/*	if (!ACPIDriver_Constructor(&self->ACPI))
	{
		return false;
	}
	*/

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

bool Kernel_InitObjects(struct Kernel* self)
{
	if (!StorageManager_Constructor(&self->StorageManager))
	{
		return false;
	}

	//PS2Driver_WaitForKeyPress("Kernel: Object init complete");
	return true;
}

void Kernel_StackTrace(unsigned int MaxFrames)
{
    struct StackFrame *stk;
    asm ("movl %%ebp,%0" : "=r"(stk) ::);
    printf("Stack trace:\n");
    for(unsigned int frame = 0; stk && frame < MaxFrames; ++frame)
    {
        // Unwind to previous stack frame
        printf("  0x%x     \n", stk->eip);
        stk = stk->ebp;
    }
}