#include "Interrupt.h"
#include "../Screen/Screen.h"
#include "../IO/IO.h"

void tkInterrupt_WriteDescriptorTable()
{
	tkScreen_PrintLine("Initialising Interrupt Description Table");
	/*   Ports
	*	 		PIC1		PIC2
	*	Command 0x20		0xA0
	*	Data	0x21		0xA1
	*/
	/* ICW1 - begin initialization */
	tkIO_WritePort(0x20 , 0x11);
	tkIO_WritePort(0xA0 , 0x11);
	/* ICW2 - remap offset address of InterruptDescriptorTable */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	tkIO_WritePort(0x21 , 0x20);
	tkIO_WritePort(0xA1 , 0x28);
	/* ICW3 - setup cascading */
	tkIO_WritePort(0x21 , 0x00);
	tkIO_WritePort(0xA1 , 0x00);
	/* ICW4 - environment info */
	tkIO_WritePort(0x21 , 0x01);
	tkIO_WritePort(0xA1 , 0x01);
	/* Initialization finished */
	/* mask interrupts */
	tkIO_WritePort(0x21 , 0xff);
	tkIO_WritePort(0xA1 , 0xff);
	/* fill the InterruptDescriptorTable descriptor */
	tkInterrupt_LIDT(InterruptDescriptorTable, sizeof(tkInterrupt_DescriptorTableEntry)*IDT_SIZE);
	tkInterrupt_STI();
}

void tkInterrupt_LIDT(void* base, uint16_t size)
{   // This function works in 32 and 64bit mode
    struct 
	{
        uint16_t length;
        void* base;
    } __attribute__((packed)) IDTR = { size, base };
    __asm__ volatile ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

void tkInterrupt_STI()
{
	//Enables the interrupts , set the interrupt flag
	__asm__ ("sti"); 
}

void tkInterrupt_CLI()
{
	//Disables interrupts ,clears the interrupt flag
	__asm__ ("cli");
}