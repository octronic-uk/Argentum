#include "Kernel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Drivers/FPU/FPUDriver.h>
#include <Drivers/Serial/Test/SerialDriverTest.h>
#include <Objects/GraphicsManager/Test/GraphicsManagerTest.h>
#include <Objects/StorageManager/Test/StorageManagerTest.h>
#include <Objects/InputManager/Test/InputManagerTest.h>
#include <Objects/Lua/lua.h>

bool Kernel_Constructor(Kernel* self, multiboot_info_t* mbi)
{
	memset(self,0,sizeof(Kernel));
	self->MultibootInfo = mbi;
	return true;
}

bool Kernel_Init(Kernel* self)
{
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

	return true;
}



bool Kernel_InitDrivers(Kernel* self)
{
	if(!TextModeDriver_Constructor(&self->TextMode))
	{
		return false;
	}

	if (!SerialDriver_Constructor(&self->Serial))
	{
		return false;
	}

	if (!VgaDriver_Constructor(&self->Vga))
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

	FPUDriver_EnableFPU();

	if (!PITDriver_Constructor(&self->PIT))
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

    if (!InputManager_Constructor(&self->InputManager))
	{
		return false;
	}

	/*
	if (!GraphicsManager_Constructor(&self->GraphicsManager))
	{
		return false;
	}
	*/

	return true;
}

void Kernel_Destructor(Kernel* self)
{
	Kernel_DestroyObjects(self);
	Kernel_DestroyDrivers(self);
}

void Kernel_DestroyObjects(Kernel* self)
{
	InputManager_Destructor(&self->InputManager);
	GraphicsManager_Destructor(&self->GraphicsManager);
	StorageManager_Destructor(&self->StorageManager);
}

void Kernel_DestroyDrivers(Kernel* self)
{
	ACPIDriver_Destructor(&self->ACPI);
}

void Kernel_Test(Kernel* self)
{
	Kernel_TestDrivers(self);
	Kernel_TestObjects(self);
}

void Kernel_TestDrivers(Kernel* self)
{
	SerialDriverTest_RunSuite(&self->Serial);
}

void Kernel_TestObjects(Kernel* self)
{
	StorageManagerTest_RunSuite(&self->StorageManager);
	InputManagerTest_RunSuite(&self->InputManager);
	GraphicsManagerTest_RunSuite(&self->GraphicsManager);
}

void Kernel_Run(Kernel* self)
{
	printf("Kernel: Entering Runloop\n");
	lua_runner();
	/*
	self->RunLoop = true;
	while(self->RunLoop)
	{
		InputManager_ProcessEvents(&self->InputManager);
		GraphicsManager_Render(&self->GraphicsManager);
	}
	*/
}