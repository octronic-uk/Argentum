#pragma once
#include <stdint.h>

#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

struct Kernel;

typedef struct
{
	uint16_t mOffsetLowerBits;
	uint16_t mSelector;
	uint8_t  mZero;
	uint8_t  mTypeAttribute;
	uint16_t mOffsetHigherBits;
} __attribute__((packed)) Interrupt_DescriptorTableEntry;

static Interrupt_DescriptorTableEntry Interrupt_DescriptorTable[IDT_SIZE];

void Interrupt_Constructor();
void Interrupt_SetIDTEntry(uint8_t index, const Interrupt_DescriptorTableEntry idt);
void Interrupt_WriteDescriptorTable();
void Interrupt_lidt(void* base, uint16_t size);
void Interrupt_sti();
void Interrupt_cli();





