#include "Interrupt.h"
#include "../Screen/Screen.h"

void tkInterruptWriteDescriptorTable()
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

	tkInterruptLIDT(InterruptDescriptorTable, sizeof(tkInterruptDescriptorTableEntry)*IDT_SIZE);
	tkInterruptSTI();
}

/* Input a byte from a port */
unsigned char tkInterruptReadPort(unsigned short port)
{
   unsigned char ret;
   asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
   return ret;
}

/* Output a byte to a port */
void tkInterruptWritePort(unsigned short port,unsigned char value)
{
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

void tkInterruptLIDT(void* base, unsigned short size)
{   // This function works in 32 and 64bit mode
    struct 
	{
        unsigned short length;
        void* base;
    } __attribute__((packed)) IDTR = { size, base };
 
    __asm__ volatile ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

void tkInterruptSTI()
{
	//Enables the interrupts , set the interrupt flag
	__asm__ ("sti"); 
}

void tkInterruptCLI()
{
	//Disables interrupts ,clears the interrupt flag
	__asm__ ("cli");
}