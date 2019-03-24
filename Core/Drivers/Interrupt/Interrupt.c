#include "Interrupt.h"
#include <Kernel.h>
#include <Drivers/IO/IO.h>

#include <stdio.h>
#include <string.h>

Interrupt_DescriptorTableEntry Interrupt_DescriptorTable[INTERRUPT_IDT_SIZE];

uint8_t Interrupt_Debug;

void Interrupt_SetDebug(uint8_t debug)
{
	Interrupt_Debug = debug;
}

uint8_t Interrupt_GetDebug()
{
	return Interrupt_Debug;
}

void Interrupt_Constructor()
{
	if (Interrupt_Debug)
	{
		printf("Interrupt: Constructing\n");
	}
    memset(&Interrupt_DescriptorTable[0],0,sizeof(Interrupt_DescriptorTableEntry)*INTERRUPT_IDT_SIZE);

	/* ICW1 - begin initialization */
	IO_WritePort8b(INTERRUPT_PIC1_COMMAND, INTERRUPT_PIC_CMD_INIT);
	IO_WritePort8b(INTERRUPT_PIC2_COMMAND, INTERRUPT_PIC_CMD_INIT);

	/* ICW2 - remap offset address of InterruptDescriptorTable */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	IO_WritePort8b(INTERRUPT_PIC1_DATA, 0x20);
	IO_WritePort8b(INTERRUPT_PIC2_DATA, 0x28);

	/* ICW3 - setup cascading */
	IO_WritePort8b(INTERRUPT_PIC1_DATA, 0x04);
	IO_WritePort8b(INTERRUPT_PIC2_DATA, 0x02);

	/* ICW4 - environment info */
	/* 0x01 = 8086 mode */
	IO_WritePort8b(INTERRUPT_PIC1_DATA, 0x01);
	IO_WritePort8b(INTERRUPT_PIC2_DATA, 0x01);

	/* Initialization finished */

	/* disable interrupts */
	Interrupt_SetMask_PIC1(0xff);
	Interrupt_SetMask_PIC2(0xff);
}

void Interrupt_WriteDescriptorTable()
{
	if (Interrupt_Debug) 
	{
		printf("Interrupt: Writing Description Table\n");
	}
	/* fill the InterruptDescriptorTable descriptor */
	Interrupt_lidt(&Interrupt_DescriptorTable[0], sizeof(Interrupt_DescriptorTableEntry)*INTERRUPT_IDT_SIZE);
}

void Interrupt_lidt(void* base, uint16_t size)
{
	if (Interrupt_Debug) 
	{
    	printf("Interrupt: LIDT\n");
	}
    // This function works in 32 and 64bit mode
	struct
	{
		uint16_t length;
		void* base;
	} __attribute__((packed)) IDTR = { size, base };
	__asm__ volatile ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

void Interrupt_Enable_STI()
{
    if (Interrupt_Debug) 
	{
    	printf("Interrupt: STI\n");
	}
	//Enables the interrupts , set the interrupt flag
	__asm__ ("sti");
}

void Interrupt_Disable_CLI()
{
    if (Interrupt_Debug) 
	{
    	printf("Interrupt: CLI\n");
	}
	//Disables interrupts ,clears the interrupt flag
	__asm__ ("cli");
}

void Interrupt_SetIDTEntry(uint8_t index, const Interrupt_DescriptorTableEntry idt)
{
    if (Interrupt_Debug) 
	{
    	printf("Interrupt: Set IDT Entry 0x%x\n",index);
	}
	Interrupt_DescriptorTable[index] = idt;
}

void Interrupt_SendEOI(uint8_t irq)
{
    if (Interrupt_Debug) 
	{
    	printf("Interrupt: Sending EOI to irq %d\n",irq);
	}
	if(irq >= 8) Interrupt_SendEOI_PIC2();
	else         Interrupt_SendEOI_PIC1();
}

void Interrupt_SendEOI_PIC1()
{
	if (Interrupt_Debug) 
	{
    	printf("Interrupt: Sending EOI to PIC 1\n");
	}
	IO_WritePort8b(INTERRUPT_PIC1_COMMAND, INTERRUPT_PIC_EOI);
}

void Interrupt_SendEOI_PIC2()
{
	if (Interrupt_Debug) 
	{
    	printf("Interrupt: Sending EOI to PIC 2\n");
	}
	IO_WritePort8b(INTERRUPT_PIC2_COMMAND, INTERRUPT_PIC_EOI);
}

void Interrupt_SetMask_PIC1(uint8_t mask)
{
	if (Interrupt_Debug) 
	{
    	printf("Interrupt: Setting mask of PIC 1 0x%x\n",mask);
	}
	IO_WritePort8b(INTERRUPT_PIC1_DATA, mask);
}

void Interrupt_SetMask_PIC2(uint8_t mask)
{
	if (Interrupt_Debug) 
	{
    	printf("Interrupt: Setting mask of PIC 2 0x%x\n",mask);
	}
	IO_WritePort8b(INTERRUPT_PIC2_DATA, mask);
}

uint8_t Interrupt_ReadISR_PIC1() 
{
	IO_WritePort8b(INTERRUPT_PIC1_COMMAND, INTERRUPT_PIC_READ_ISR);
	uint8_t isr = IO_ReadPort8b(INTERRUPT_PIC1_COMMAND);
	return isr;
}

uint8_t Interrupt_ReadISR_PIC2() 
{
	IO_WritePort8b(INTERRUPT_PIC2_COMMAND, INTERRUPT_PIC_READ_ISR);
	uint8_t isr = IO_ReadPort8b(INTERRUPT_PIC2_COMMAND);
	return isr;
}

uint8_t Interrupt_ReadIRR_PIC1() 
{
	IO_WritePort8b(INTERRUPT_PIC1_COMMAND, INTERRUPT_PIC_READ_IRR);
	uint8_t irr = IO_ReadPort8b(INTERRUPT_PIC1_COMMAND);
	return irr;
}

uint8_t Interrupt_ReadIRR_PIC2() 
{
	IO_WritePort8b(INTERRUPT_PIC2_COMMAND, INTERRUPT_PIC_READ_IRR);
	uint8_t irr = IO_ReadPort8b(INTERRUPT_PIC2_COMMAND);
	return irr;
}
