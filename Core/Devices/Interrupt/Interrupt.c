#include "Interrupt.h"
#include <Kernel.h>
#include <Devices/IO/IO.h>

#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

void Interrupt_Constructor()
{
    memset(&Interrupt_DescriptorTable[0],0,sizeof(Interrupt_DescriptorTableEntry)*IDT_SIZE);
}

void Interrupt_WriteDescriptorTable()
{
	#ifdef __DEBUG_INTERRUPT
	printf("Interrupt: Writing Description Table\n");
	#endif
	/*   Ports
	*	 		PIC1		PIC2
	*	Command 0x20		0xA0
	*	Data	0x21		0xA1
	*/
	/* ICW1 - begin initialization */
	IO_WritePort(0x20 , 0x11);
	IO_WritePort(0xA0 , 0x11);
	/* ICW2 - remap offset address of InterruptDescriptorTable */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	IO_WritePort(0x21 , 0x20);
	IO_WritePort(0xA1 , 0x28);
	/* ICW3 - setup cascading */
	IO_WritePort(0x21 , 0x00);
	IO_WritePort(0xA1 , 0x00);
	/* ICW4 - environment info */
	IO_WritePort(0x21 , 0x01);
	IO_WritePort(0xA1 , 0x01);
	/* Initialization finished */
	/* mask interrupts */
	IO_WritePort(0x21 , 0xff);
	IO_WritePort(0xA1 , 0xff);
	/* fill the InterruptDescriptorTable descriptor */
	Interrupt_lidt(&Interrupt_DescriptorTable[0], sizeof(Interrupt_DescriptorTableEntry)*IDT_SIZE);
	Interrupt_sti();
}

void Interrupt_lidt(void* base, uint16_t size)
{

	#ifdef __DEBUG_INTERRUPT
    printf("Interrupt: LIDT\n");
	#endif
    // This function works in 32 and 64bit mode
	struct
	{
		uint16_t length;
		void* base;
	} __attribute__((packed)) IDTR = { size, base };
	__asm__ volatile ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

void Interrupt_sti()
{
    #ifdef __DEBUG_INTERRUPT
    printf("Interrupt: STI\n");
	#endif
	//Enables the interrupts , set the interrupt flag
	__asm__ ("sti");
}

void Interrupt_cli()
{
    #ifdef __DEBUG_INTERRUPT
    printf("Interrupt: CLI\n");
	#endif
	//Disables interrupts ,clears the interrupt flag
	__asm__ ("cli");
}

void Interrupt_SetIDTEntry(uint8_t index, const Interrupt_DescriptorTableEntry idt)
{
    #ifdef __DEBUG_INTERRUPT
    printf("Interrupt: Set IDT Entry %d\n",index);
	#endif
	Interrupt_DescriptorTable[index] = idt;
}
