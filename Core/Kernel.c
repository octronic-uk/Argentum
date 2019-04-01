#include <Kernel.h>
#include <stdio.h>
#include <unistd.h>

void Kernel_Constructor(struct Kernel* self, multiboot_info_t* mbi)
{
	Screen_Constructor(&self->Screen);
	printf("Taskie Kernel 1.0\n");
	Memory_Constructor(&self->Memory, mbi);
	Interrupt_Constructor(&self->Interrupt);
	Serial_Constructor(&self->Serial,&self->Interrupt);
	Serial_TestPort1(&self->Serial);
	//printf_to_serial();

	PS2_Constructor(&self->PS2,&self->Interrupt);
	
	ACPI_Constructor(&self->ACPI,&self->Interrupt, &self->Memory);
	PCI_Constructor(&self->PCI,&self->Memory);

	ATA_Constructor(&self->ATA,&self->PCI);

    PS2_WaitForKeyPress(&self->PS2);

	Kernel_InitStorageManager(self);
}

void
Kernel_InitStorageManager(struct Kernel* self)
{
	printf("Kernel: Initialising Storage Manager %s\n",self->ATA.IDEDevices[0].model);
	struct FatVolume part;
	FatVolume_Constructor(&part, &self->ATA, 0,0);
}