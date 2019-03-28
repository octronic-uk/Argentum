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
#define INTERRUPT_HANDLER_FUNCTIONS_COUNT 16

extern int irq0();
extern int irq1();
extern int irq2();
extern int irq3();
extern int irq4();
extern int irq5();
extern int irq6();
extern int irq7();
extern int irq8();
extern int irq9();
extern int irq10();
extern int irq11();
extern int irq12();
extern int irq13();
extern int irq14();
extern int irq15();

typedef struct
{
	uint16_t OffsetLowerBits;
	uint16_t Selector;
	uint8_t  Zero;
	uint8_t  TypeAttribute;
	uint16_t OffsetHigherBits;
} __attribute__((packed)) Interrupt_DescriptorTableEntry;

void Interrupt_Constructor();
void Interrupt_SetHandlerFunction(uint8_t index, void(*fn)(void));
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

void Interrupt_SetupDescriptorTable();
void irq0_handler();
void irq1_handler();
void irq2_handler();
void irq3_handler();
void irq4_handler();
void irq5_handler();
void irq6_handler();
void irq7_handler();
void irq8_handler();
void irq9_handler();
void irq10_handler();
void irq11_handler();
void irq12_handler();
void irq13_handler();
void irq14_handler();
void irq15_handler();