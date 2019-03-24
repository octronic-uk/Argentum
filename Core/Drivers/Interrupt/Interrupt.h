#pragma once
#include <stdint.h>

#define INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET 0x08
#define INTERRUPT_IDT_SIZE 		256
#define INTERRUPT_GATE 			0x8e
#define INTERRUPT_PIC1			0x20		/* IO base address for master PIC */
#define INTERRUPT_PIC2			0xA0		/* IO base address for slave PIC */
#define INTERRUPT_PIC1_COMMAND	INTERRUPT_PIC1
#define INTERRUPT_PIC1_DATA		(INTERRUPT_PIC1+1)
#define INTERRUPT_PIC2_COMMAND	INTERRUPT_PIC2
#define INTERRUPT_PIC2_DATA		(INTERRUPT_PIC2+1)
#define INTERRUPT_PIC_EOI		0x20		/* End-of-interrupt command code */
#define INTERRUPT_PIC_CMD_INIT  0x11
#define INTERRUPT_PIC_READ_IRR  0x0a    /* OCW3 irq ready next CMD read */
#define INTERRUPT_PIC_READ_ISR  0x0b    /* OCW3 irq service next CMD read */

struct Kernel;

typedef struct
{
	uint16_t mOffsetLowerBits;
	uint16_t mSelector;
	uint8_t  mZero;
	uint8_t  mTypeAttribute;
	uint16_t mOffsetHigherBits;
} __attribute__((packed)) Interrupt_DescriptorTableEntry;

void Interrupt_SetDebug(uint8_t debug);
uint8_t Interrupt_GetDebug();

void Interrupt_Constructor();
void Interrupt_SetIDTEntry(uint8_t index, const Interrupt_DescriptorTableEntry idt);
void Interrupt_WriteDescriptorTable();
void Interrupt_lidt(void* base, uint16_t size);
void Interrupt_Enable_STI();
void Interrupt_Disable_CLI();
void Interrupt_SendEOI(uint8_t irq);
void Interrupt_SendEOI_PIC1();
void Interrupt_SendEOI_PIC2();
void Interrupt_SetMask_PIC1(uint8_t mask);
void Interrupt_SetMask_PIC2(uint8_t mask);
uint8_t Interrupt_ReadISR_PIC1();
uint8_t Interrupt_ReadISR_PIC2();
uint8_t Interrupt_ReadIRR_PIC1();
uint8_t Interrupt_ReadIRR_PIC2();




