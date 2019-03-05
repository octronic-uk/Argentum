#include "Interrupt.h"
#include "../Screen/Screen.h"

void tkInterruptInitialiseDescriptorTable()
{
	tkScreenPrintLine("Initialising Interrupt Description Table");

	/*   Ports
	*	 		PIC1		PIC2
	*	Command 0x20		0xA0
	*	Data	0x21		0xA1
	*/

	/* ICW1 - begin initialization */
	tkInterruptWritePort(0x20 , 0x11);
	tkInterruptWritePort(0xA0 , 0x11);

	/* ICW2 - remap offset address of InterruptDescriptorTable */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	tkInterruptWritePort(0x21 , 0x20);
	tkInterruptWritePort(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	tkInterruptWritePort(0x21 , 0x00);
	tkInterruptWritePort(0xA1 , 0x00);

	/* ICW4 - environment info */
	tkInterruptWritePort(0x21 , 0x01);
	tkInterruptWritePort(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	tkInterruptWritePort(0x21 , 0xff);
	tkInterruptWritePort(0xA1 , 0xff);

	/* fill the InterruptDescriptorTable descriptor */
	unsigned long idt_address,  idt_ptr[2];
	idt_address = (unsigned long)InterruptDescriptorTable;
	idt_ptr[0] = (sizeof (struct tkInterruptDescriptorTableEntry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	tkInterruptLoadIDT(idt_ptr);
}