#pragma once

#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

typedef struct 
{
	unsigned short int mOffsetLowerBits;
	unsigned short int mSelector;
	unsigned char mZero;
	unsigned char mTypeAttribute;
	unsigned short int mOffsetHigherBits;
} __attribute__((packed)) tkInterruptDescriptorTableEntry;

tkInterruptDescriptorTableEntry InterruptDescriptorTable[IDT_SIZE];

unsigned char tkInterruptReadPort(unsigned short port);
void tkInterruptWritePort(unsigned short port,unsigned char value);
void tkInterruptWriteDescriptorTable();
void tkInterruptLIDT(void* base, unsigned short size);
void tkInterruptSTI();
void tkInterruptCLI();