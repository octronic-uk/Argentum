#include "Interrupt.h"
#include <Kernel.h>
#include <Drivers/IO/IO.h>

#include <string.h>

Interrupt_DescriptorTableEntry Interrupt_DescriptorTable[INTERRUPT_IDT_SIZE];
void(*Interrupt_HandlerFunctions[INTERRUPT_HANDLER_FUNCTIONS_COUNT])(void) ;

void Interrupt_Constructor()
{
	memset(Interrupt_HandlerFunctions,0,sizeof(void*)*INTERRUPT_HANDLER_FUNCTIONS_COUNT);
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

	Interrupt_SetupDescriptorTable();
	Interrupt_WriteDescriptorTable();

	Interrupt_SetMask_PIC1(0x01);
	Interrupt_SetMask_PIC2(0x00);

	Interrupt_Enable_STI();
}

void Interrupt_WriteDescriptorTable()
{
	/* fill the InterruptDescriptorTable descriptor */
	Interrupt_lidt(&Interrupt_DescriptorTable[0], sizeof(Interrupt_DescriptorTableEntry)*INTERRUPT_IDT_SIZE);
}

void Interrupt_lidt(void* base, uint16_t size)
{
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
	//Enables the interrupts , set the interrupt flag
	__asm__ ("sti");
}

void Interrupt_Disable_CLI()
{
	//Disables interrupts ,clears the interrupt flag
	__asm__ ("cli");
}

void Interrupt_SetHandlerFunction(uint8_t index, void(*fn)(void))
{
	Interrupt_HandlerFunctions[index] = fn;
}

void Interrupt_SendEOI(uint8_t irq)
{
	if(irq >= 8) Interrupt_SendEOI_PIC2();
	else         Interrupt_SendEOI_PIC1();
}

void Interrupt_SendEOI_PIC1()
{
	IO_WritePort8b(INTERRUPT_PIC1_COMMAND, INTERRUPT_PIC_EOI);
}

void Interrupt_SendEOI_PIC2()
{
	IO_WritePort8b(INTERRUPT_PIC2_COMMAND, INTERRUPT_PIC_EOI);
}

void Interrupt_SetMask_PIC1(uint8_t mask)
{
	IO_WritePort8b(INTERRUPT_PIC1_DATA, mask);
}

void Interrupt_SetMask_PIC2(uint8_t mask)
{
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

void Interrupt_SetupDescriptorTable()
{
	unsigned long irq0_address;
	unsigned long irq1_address;
	unsigned long irq2_address;
	unsigned long irq3_address;          
	unsigned long irq4_address; 
	unsigned long irq5_address;
	unsigned long irq6_address;
	unsigned long irq7_address;
	unsigned long irq8_address;
	unsigned long irq9_address;          
	unsigned long irq10_address;
	unsigned long irq11_address;
	unsigned long irq12_address;
	unsigned long irq13_address;
	unsigned long irq14_address;          
	unsigned long irq15_address;         
	unsigned long idt_address;
 
	irq0_address = (unsigned long)irq0; 
	Interrupt_DescriptorTable[32].OffsetLowerBits = irq0_address & 0xffff;
	Interrupt_DescriptorTable[32].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[32].Zero = 0;
	Interrupt_DescriptorTable[32].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[32].OffsetHigherBits = (irq0_address & 0xffff0000) >> 16;
 
	irq1_address = (unsigned long)irq1; 
	Interrupt_DescriptorTable[33].OffsetLowerBits = irq1_address & 0xffff;
	Interrupt_DescriptorTable[33].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[33].Zero = 0;
	Interrupt_DescriptorTable[33].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[33].OffsetHigherBits = (irq1_address & 0xffff0000) >> 16;
 
	irq2_address = (unsigned long)irq2; 
	Interrupt_DescriptorTable[34].OffsetLowerBits = irq2_address & 0xffff;
	Interrupt_DescriptorTable[34].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[34].Zero = 0;
	Interrupt_DescriptorTable[34].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[34].OffsetHigherBits = (irq2_address & 0xffff0000) >> 16;
 
	irq3_address = (unsigned long)irq3; 
	Interrupt_DescriptorTable[35].OffsetLowerBits = irq3_address & 0xffff;
	Interrupt_DescriptorTable[35].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[35].Zero = 0;
	Interrupt_DescriptorTable[35].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[35].OffsetHigherBits = (irq3_address & 0xffff0000) >> 16;
 
	irq4_address = (unsigned long)irq4; 
	Interrupt_DescriptorTable[36].OffsetLowerBits = irq4_address & 0xffff;
	Interrupt_DescriptorTable[36].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[36].Zero = 0;
	Interrupt_DescriptorTable[36].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[36].OffsetHigherBits = (irq4_address & 0xffff0000) >> 16;
 
	irq5_address = (unsigned long)irq5; 
	Interrupt_DescriptorTable[37].OffsetLowerBits = irq5_address & 0xffff;
	Interrupt_DescriptorTable[37].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[37].Zero = 0;
	Interrupt_DescriptorTable[37].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[37].OffsetHigherBits = (irq5_address & 0xffff0000) >> 16;
 
	irq6_address = (unsigned long)irq6; 
	Interrupt_DescriptorTable[38].OffsetLowerBits = irq6_address & 0xffff;
	Interrupt_DescriptorTable[38].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[38].Zero = 0;
	Interrupt_DescriptorTable[38].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[38].OffsetHigherBits = (irq6_address & 0xffff0000) >> 16;
 
	irq7_address = (unsigned long)irq7; 
	Interrupt_DescriptorTable[39].OffsetLowerBits = irq7_address & 0xffff;
	Interrupt_DescriptorTable[39].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[39].Zero = 0;
	Interrupt_DescriptorTable[39].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[39].OffsetHigherBits = (irq7_address & 0xffff0000) >> 16;
 
	irq8_address = (unsigned long)irq8; 
	Interrupt_DescriptorTable[40].OffsetLowerBits = irq8_address & 0xffff;
	Interrupt_DescriptorTable[40].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[40].Zero = 0;
	Interrupt_DescriptorTable[40].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[40].OffsetHigherBits = (irq8_address & 0xffff0000) >> 16;
 
	irq9_address = (unsigned long)irq9; 
	Interrupt_DescriptorTable[41].OffsetLowerBits = irq9_address & 0xffff;
	Interrupt_DescriptorTable[41].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[41].Zero = 0;
	Interrupt_DescriptorTable[41].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[41].OffsetHigherBits = (irq9_address & 0xffff0000) >> 16;
 
	irq10_address = (unsigned long)irq10; 
	Interrupt_DescriptorTable[42].OffsetLowerBits = irq10_address & 0xffff;
	Interrupt_DescriptorTable[42].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[42].Zero = 0;
	Interrupt_DescriptorTable[42].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[42].OffsetHigherBits = (irq10_address & 0xffff0000) >> 16;
 
	irq11_address = (unsigned long)irq11; 
	Interrupt_DescriptorTable[43].OffsetLowerBits = irq11_address & 0xffff;
	Interrupt_DescriptorTable[43].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[43].Zero = 0;
	Interrupt_DescriptorTable[43].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[43].OffsetHigherBits = (irq11_address & 0xffff0000) >> 16;
 
	irq12_address = (unsigned long)irq12; 
	Interrupt_DescriptorTable[44].OffsetLowerBits = irq12_address & 0xffff;
	Interrupt_DescriptorTable[44].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[44].Zero = 0;
	Interrupt_DescriptorTable[44].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[44].OffsetHigherBits = (irq12_address & 0xffff0000) >> 16;
 
	irq13_address = (unsigned long)irq13; 
	Interrupt_DescriptorTable[45].OffsetLowerBits = irq13_address & 0xffff;
	Interrupt_DescriptorTable[45].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[45].Zero = 0;
	Interrupt_DescriptorTable[45].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[45].OffsetHigherBits = (irq13_address & 0xffff0000) >> 16;
 
	irq14_address = (unsigned long)irq14; 
	Interrupt_DescriptorTable[46].OffsetLowerBits = irq14_address & 0xffff;
	Interrupt_DescriptorTable[46].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[46].Zero = 0;
	Interrupt_DescriptorTable[46].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[46].OffsetHigherBits = (irq14_address & 0xffff0000) >> 16;
 
    irq15_address = (unsigned long)irq15; 
	Interrupt_DescriptorTable[47].OffsetLowerBits = irq15_address & 0xffff;
	Interrupt_DescriptorTable[47].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Interrupt_DescriptorTable[47].Zero = 0;
	Interrupt_DescriptorTable[47].TypeAttribute = INTERRUPT_GATE;
	Interrupt_DescriptorTable[47].OffsetHigherBits = (irq15_address & 0xffff0000) >> 16;
 
}

void irq0_handler() 
{
	if (Interrupt_HandlerFunctions[0])
	{
		Interrupt_HandlerFunctions[0]();
	}
    Interrupt_SendEOI_PIC1();
}
 
void irq1_handler() 
{
	if (Interrupt_HandlerFunctions[1])
	{
		Interrupt_HandlerFunctions[1]();
	}
	Interrupt_SendEOI_PIC1();
}

void irq2_handler() 
{
	if (Interrupt_HandlerFunctions[2])
	{
		Interrupt_HandlerFunctions[2]();
	}
	Interrupt_SendEOI_PIC1();
}

void irq3_handler() 
{
	if (Interrupt_HandlerFunctions[3])
	{
		Interrupt_HandlerFunctions[3]();
	}
	Interrupt_SendEOI_PIC1();
}

void irq4_handler() 
{
	if (Interrupt_HandlerFunctions[4])
	{
		Interrupt_HandlerFunctions[4]();
	}
	Interrupt_SendEOI_PIC1();
}

void irq5_handler() 
{
	if (Interrupt_HandlerFunctions[5])
	{
		Interrupt_HandlerFunctions[5]();
	}
	Interrupt_SendEOI_PIC1();
}

void irq6_handler() 
{
	if (Interrupt_HandlerFunctions[6])
	{
		Interrupt_HandlerFunctions[6]();
	}
	Interrupt_SendEOI_PIC1();
}

void irq7_handler() 
{
	if (Interrupt_HandlerFunctions[7])
	{
		Interrupt_HandlerFunctions[7]();
	}
	Interrupt_SendEOI_PIC1();
}

void irq8_handler() 
{
	if (Interrupt_HandlerFunctions[8])
	{
		Interrupt_HandlerFunctions[8]();
	}
	Interrupt_SendEOI_PIC2();
	Interrupt_SendEOI_PIC1();          
}
 
void irq9_handler() 
{
	if (Interrupt_HandlerFunctions[9])
	{
		Interrupt_HandlerFunctions[9]();
	}
	Interrupt_SendEOI_PIC2();
	Interrupt_SendEOI_PIC1();
}

void irq10_handler() 
{
	if (Interrupt_HandlerFunctions[10])
	{
		Interrupt_HandlerFunctions[10]();
	}
	Interrupt_SendEOI_PIC2();
	Interrupt_SendEOI_PIC1();
}
 
void irq11_handler() 
{
	if (Interrupt_HandlerFunctions[11])
	{
		Interrupt_HandlerFunctions[11]();
	}
	Interrupt_SendEOI_PIC2();
	Interrupt_SendEOI_PIC1();
}
 
void irq12_handler() 
{
	if (Interrupt_HandlerFunctions[12])
	{
		Interrupt_HandlerFunctions[12]();
	}
	Interrupt_SendEOI_PIC2();
	Interrupt_SendEOI_PIC1();
}
 
void irq13_handler() 
{
	if (Interrupt_HandlerFunctions[13])
	{
		Interrupt_HandlerFunctions[13]();
	}
	Interrupt_SendEOI_PIC2();
	Interrupt_SendEOI_PIC1();
}
 
void irq14_handler() 
{
	if (Interrupt_HandlerFunctions[14])
	{
		Interrupt_HandlerFunctions[14]();
	}
	Interrupt_SendEOI_PIC2();
	Interrupt_SendEOI_PIC1();
}
 
void irq15_handler() 
{
	if (Interrupt_HandlerFunctions[15])
	{
		Interrupt_HandlerFunctions[15]();
	}
	Interrupt_SendEOI_PIC2();
	Interrupt_SendEOI_PIC1();
}