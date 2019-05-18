#include <Drivers/Interrupt/InterruptDriver.h>

#include <string.h>
#include <stdio.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/IO/IODriver.h>

extern Kernel _Kernel;

bool InterruptDriver_Constructor(InterruptDriver* self)
{
	printf("Interrupt Driver: Constructing\n");
	self->Debug = false;
	InterruptDriver_Disable_CLI(self);

	memset(self->HandlerFunctions,0,sizeof(void*)*INTERRUPT_HANDLER_FUNCTIONS_COUNT);
    memset(self->DescriptorTable,0,sizeof(InterruptDescriptorTableEntry)*INTERRUPT_IDT_SIZE);

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
	InterruptDriver_SetMask_PIC1(self, 0xff);
	InterruptDriver_SetMask_PIC2(self, 0xff);

	InterruptDriver_SetupDescriptorTable(self);
	InterruptDriver_WriteDescriptorTable(self);

	return true;
}

void InterruptDriver_WriteDescriptorTable(InterruptDriver* self)
{
	if (self->Debug)
	{
		printf("Interrupt: Writing Descriptor Table\n");
	}
	/* fill the InterruptDescriptorTable descriptor */
	InterruptDriver_lidt(self, &self->DescriptorTable[0], sizeof(InterruptDescriptorTableEntry)*INTERRUPT_IDT_SIZE);
}

void InterruptDriver_lidt(InterruptDriver* self, void* base, uint16_t size)
{
	if (self->Debug) printf("Interrupt: LIDT\n");
    // This function works in 32 and 64bit mode
	struct
	{
		uint16_t length;
		void* base;
	} __attribute__((packed)) IDTR = { size, base };
	__asm__ volatile ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

void InterruptDriver_Enable_STI(InterruptDriver* self)
{
	if (self->Debug) printf("Interrupt: STI\n");
	//Enables the interrupts , set the interrupt flag
	__asm__ ("sti");
}

void InterruptDriver_Disable_CLI(InterruptDriver* self)
{
	if (self->Debug) printf("Interrupt: CLI\n");
	//Disables interrupts ,clears the interrupt flag
	__asm__ ("cli");
}

void InterruptDriver_SetHandlerFunction(InterruptDriver* self, uint8_t index, void(*fn)(void))
{
	if (self->Debug) printf("Interrupt: Set Handler for %d\n",index);
	self->HandlerFunctions[index] = fn;
}

void InterruptDriver_SendEOI(InterruptDriver* self, uint8_t irq)
{
	if(irq >= 8) InterruptDriver_SendEOI_PIC2(self);
	else         InterruptDriver_SendEOI_PIC1(self);
}

void InterruptDriver_SendEOI_PIC1(InterruptDriver* self)
{
	//if (self->Debug) printf("Interrupt: Sending EOI to PIC1\n");
	IO_WritePort8b(INTERRUPT_PIC1_COMMAND, INTERRUPT_PIC_EOI);
}

void InterruptDriver_SendEOI_PIC2(InterruptDriver* self)
{
	if (self->Debug) printf("Interrupt: Sending EOI to PIC2\n");
	IO_WritePort8b(INTERRUPT_PIC2_COMMAND, INTERRUPT_PIC_EOI);
}

void InterruptDriver_SetMask_PIC1(InterruptDriver* self, uint8_t mask)
{
	if (self->Debug) printf("Interrupt: Setting PIC1 mask 0x%x\n",mask);
	IO_WritePort8b(INTERRUPT_PIC1_DATA, mask);
}

void InterruptDriver_SetMask_PIC2(InterruptDriver* self, uint8_t mask)
{
	if (self->Debug) printf("Interrupt: Setting PIC2 mask 0x%x\n",mask);
	IO_WritePort8b(INTERRUPT_PIC2_DATA, mask);
}

uint8_t InterruptDriver_ReadISR_PIC1(InterruptDriver* self) 
{
	IO_WritePort8b(INTERRUPT_PIC1_COMMAND, INTERRUPT_PIC_READ_ISR);
	uint8_t isr = IO_ReadPort8b(INTERRUPT_PIC1_COMMAND);
	return isr;
}

uint8_t InterruptDriver_ReadISR_PIC2(InterruptDriver* self) 
{
	IO_WritePort8b(INTERRUPT_PIC2_COMMAND, INTERRUPT_PIC_READ_ISR);
	uint8_t isr = IO_ReadPort8b(INTERRUPT_PIC2_COMMAND);
	return isr;
}

uint8_t InterruptDriver_ReadIRR_PIC1(InterruptDriver* self) 
{
	IO_WritePort8b(INTERRUPT_PIC1_COMMAND, INTERRUPT_PIC_READ_IRR);
	uint8_t irr = IO_ReadPort8b(INTERRUPT_PIC1_COMMAND);
	return irr;
}

uint8_t InterruptDriver_ReadIRR_PIC2(InterruptDriver* self) 
{
	IO_WritePort8b(INTERRUPT_PIC2_COMMAND, INTERRUPT_PIC_READ_IRR);
	uint8_t irr = IO_ReadPort8b(INTERRUPT_PIC2_COMMAND);
	return irr;
}

void InterruptDriver_SetupDescriptorTable(InterruptDriver* self)
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
	self->DescriptorTable[32].OffsetLowerBits = irq0_address & 0xffff;
	self->DescriptorTable[32].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[32].Zero = 0;
	self->DescriptorTable[32].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[32].OffsetHigherBits = (irq0_address & 0xffff0000) >> 16;
 
	irq1_address = (unsigned long)irq1; 
	self->DescriptorTable[33].OffsetLowerBits = irq1_address & 0xffff;
	self->DescriptorTable[33].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[33].Zero = 0;
	self->DescriptorTable[33].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[33].OffsetHigherBits = (irq1_address & 0xffff0000) >> 16;
 
	irq2_address = (unsigned long)irq2; 
	self->DescriptorTable[34].OffsetLowerBits = irq2_address & 0xffff;
	self->DescriptorTable[34].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[34].Zero = 0;
	self->DescriptorTable[34].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[34].OffsetHigherBits = (irq2_address & 0xffff0000) >> 16;
 
	irq3_address = (unsigned long)irq3; 
	self->DescriptorTable[35].OffsetLowerBits = irq3_address & 0xffff;
	self->DescriptorTable[35].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[35].Zero = 0;
	self->DescriptorTable[35].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[35].OffsetHigherBits = (irq3_address & 0xffff0000) >> 16;
 
	irq4_address = (unsigned long)irq4; 
	self->DescriptorTable[36].OffsetLowerBits = irq4_address & 0xffff;
	self->DescriptorTable[36].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[36].Zero = 0;
	self->DescriptorTable[36].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[36].OffsetHigherBits = (irq4_address & 0xffff0000) >> 16;
 
	irq5_address = (unsigned long)irq5; 
	self->DescriptorTable[37].OffsetLowerBits = irq5_address & 0xffff;
	self->DescriptorTable[37].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[37].Zero = 0;
	self->DescriptorTable[37].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[37].OffsetHigherBits = (irq5_address & 0xffff0000) >> 16;
 
	irq6_address = (unsigned long)irq6; 
	self->DescriptorTable[38].OffsetLowerBits = irq6_address & 0xffff;
	self->DescriptorTable[38].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[38].Zero = 0;
	self->DescriptorTable[38].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[38].OffsetHigherBits = (irq6_address & 0xffff0000) >> 16;
 
	irq7_address = (unsigned long)irq7; 
	self->DescriptorTable[39].OffsetLowerBits = irq7_address & 0xffff;
	self->DescriptorTable[39].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[39].Zero = 0;
	self->DescriptorTable[39].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[39].OffsetHigherBits = (irq7_address & 0xffff0000) >> 16;
 
	irq8_address = (unsigned long)irq8; 
	self->DescriptorTable[40].OffsetLowerBits = irq8_address & 0xffff;
	self->DescriptorTable[40].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[40].Zero = 0;
	self->DescriptorTable[40].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[40].OffsetHigherBits = (irq8_address & 0xffff0000) >> 16;
 
	irq9_address = (unsigned long)irq9; 
	self->DescriptorTable[41].OffsetLowerBits = irq9_address & 0xffff;
	self->DescriptorTable[41].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[41].Zero = 0;
	self->DescriptorTable[41].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[41].OffsetHigherBits = (irq9_address & 0xffff0000) >> 16;
 
	irq10_address = (unsigned long)irq10; 
	self->DescriptorTable[42].OffsetLowerBits = irq10_address & 0xffff;
	self->DescriptorTable[42].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[42].Zero = 0;
	self->DescriptorTable[42].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[42].OffsetHigherBits = (irq10_address & 0xffff0000) >> 16;
 
	irq11_address = (unsigned long)irq11; 
	self->DescriptorTable[43].OffsetLowerBits = irq11_address & 0xffff;
	self->DescriptorTable[43].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[43].Zero = 0;
	self->DescriptorTable[43].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[43].OffsetHigherBits = (irq11_address & 0xffff0000) >> 16;
 
	irq12_address = (unsigned long)irq12; 
	self->DescriptorTable[44].OffsetLowerBits = irq12_address & 0xffff;
	self->DescriptorTable[44].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[44].Zero = 0;
	self->DescriptorTable[44].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[44].OffsetHigherBits = (irq12_address & 0xffff0000) >> 16;
 
	irq13_address = (unsigned long)irq13; 
	self->DescriptorTable[45].OffsetLowerBits = irq13_address & 0xffff;
	self->DescriptorTable[45].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[45].Zero = 0;
	self->DescriptorTable[45].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[45].OffsetHigherBits = (irq13_address & 0xffff0000) >> 16;
 
	irq14_address = (unsigned long)irq14; 
	self->DescriptorTable[46].OffsetLowerBits = irq14_address & 0xffff;
	self->DescriptorTable[46].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[46].Zero = 0;
	self->DescriptorTable[46].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[46].OffsetHigherBits = (irq14_address & 0xffff0000) >> 16;
 
    irq15_address = (unsigned long)irq15; 
	self->DescriptorTable[47].OffsetLowerBits = irq15_address & 0xffff;
	self->DescriptorTable[47].Selector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	self->DescriptorTable[47].Zero = 0;
	self->DescriptorTable[47].TypeAttribute = INTERRUPT_GATE;
	self->DescriptorTable[47].OffsetHigherBits = (irq15_address & 0xffff0000) >> 16;
}

void irq0_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	//if (self->Debug) printf("Interrupt: 0\n");
	if (self->HandlerFunctions[0])
	{
		self->HandlerFunctions[0]();
	}
    InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}
 
void irq1_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 1\n");
	if (self->HandlerFunctions[1])
	{
		self->HandlerFunctions[1]();
	}
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}

void irq2_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 2\n");
	if (self->HandlerFunctions[2])
	{
		self->HandlerFunctions[2]();
	}
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}

void irq3_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 3\n");
	if (self->HandlerFunctions[3])
	{
		self->HandlerFunctions[3]();
	}
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}

void irq4_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 4\n");
	if (self->HandlerFunctions[4])
	{
		self->HandlerFunctions[4]();
	}
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}

void irq5_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 5\n");
	if (self->HandlerFunctions[5])
	{
		self->HandlerFunctions[5]();
	}
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}

void irq6_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 6\n");
	if (self->HandlerFunctions[6])
	{
		self->HandlerFunctions[6]();
	}
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}

void irq7_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 7\n");
	if (self->HandlerFunctions[7])
	{
		self->HandlerFunctions[7]();
	}
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}

void irq8_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 8\n");
	if (self->HandlerFunctions[8])
	{
		self->HandlerFunctions[8]();
	}
	InterruptDriver_SendEOI_PIC2(self);
	InterruptDriver_SendEOI_PIC1(self);          
	asm("sti");
}
 
void irq9_handler() 
{
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 9\n");
	if (self->HandlerFunctions[9])
	{
		self->HandlerFunctions[9]();
	}
	InterruptDriver_SendEOI_PIC2(self);
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}

void irq10_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 10\n");
	if (self->HandlerFunctions[10])
	{
		self->HandlerFunctions[10]();
	}
	InterruptDriver_SendEOI_PIC2(self);
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}
 
void irq11_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 11\n");
	if (self->HandlerFunctions[11])
	{
		self->HandlerFunctions[11]();
	}
	InterruptDriver_SendEOI_PIC2(self);
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}
 
void irq12_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 12\n");
	if (self->HandlerFunctions[12])
	{
		self->HandlerFunctions[12]();
	}
	InterruptDriver_SendEOI_PIC2(self);
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}
 
void irq13_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 13\n");
	if (self->HandlerFunctions[13])
	{
		self->HandlerFunctions[13]();
	}
	InterruptDriver_SendEOI_PIC2(self);
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}
 
void irq14_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 14\n");
	if (self->HandlerFunctions[14])
	{
		self->HandlerFunctions[14]();
	}
	InterruptDriver_SendEOI_PIC2(self);
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}
 
void irq15_handler() 
{
	asm("cli");
	struct InterruptDriver* self = &_Kernel.Interrupt;
	if (self->Debug) printf("Interrupt: 15\n");
	if (self->HandlerFunctions[15])
	{
		self->HandlerFunctions[15]();
	}
	InterruptDriver_SendEOI_PIC2(self);
	InterruptDriver_SendEOI_PIC1(self);
	asm("sti");
}