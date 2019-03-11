#pragma once

#include "../Types.h"

#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

typedef struct 
{
	uint16_t mOffsetLowerBits;
	uint16_t mSelector;
	uint8_t  mZero;
	uint8_t  mTypeAttribute;
	uint16_t mOffsetHigherBits;
} __attribute__((packed)) tkInterrupt_DescriptorTableEntry;

tkInterrupt_DescriptorTableEntry InterruptDescriptorTable[IDT_SIZE];
void tkInterrupt_WriteDescriptorTable();
void tkInterrupt_LIDT(void* base, uint16_t size);
void tkInterrupt_STI();
void tkInterrupt_CLI();