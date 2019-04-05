#pragma once

#include <stdint.h>
#include <stdbool.h>

struct Kernel;

#define INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET 0x08
#define INTERRUPT_IDT_SIZE 		256
#define INTERRUPT_GATE 			0x8e
#define INTERRUPT_PIC1			0x20		/* IO base address for master PIC */
#define INTERRUPT_PIC2			0xA0		/* IO base address for slave PIC */
#define INTERRUPT_PIC1_COMMAND	0x20
#define INTERRUPT_PIC1_DATA		0x21
#define INTERRUPT_PIC2_COMMAND	0xA0
#define INTERRUPT_PIC2_DATA		0xA1
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

struct Interrupt_DescriptorTableEntry
{
	uint16_t OffsetLowerBits;
	uint16_t Selector;
	uint8_t  Zero;
	uint8_t  TypeAttribute;
	uint16_t OffsetHigherBits;
} __attribute__((packed));

struct InterruptDriver
{
	struct Kernel* Kernel;
	bool Debug;
	struct Interrupt_DescriptorTableEntry DescriptorTable[INTERRUPT_IDT_SIZE];
	void(*HandlerFunctions[INTERRUPT_HANDLER_FUNCTIONS_COUNT])(void);
};

bool InterruptDriver_Constructor(struct InterruptDriver* self, struct Kernel* kernel);
void InterruptDriver_SetHandlerFunction(struct InterruptDriver* self, uint8_t index, void(*fn)(void));
void InterruptDriver_WriteDescriptorTable(struct InterruptDriver* self);
void InterruptDriver_lidt(struct InterruptDriver* self, void* base, uint16_t size);
void InterruptDriver_Enable_STI(struct InterruptDriver* self);
void InterruptDriver_Disable_CLI(struct InterruptDriver* self);
void InterruptDriver_SendEOI(struct InterruptDriver* self, uint8_t irq);
void InterruptDriver_SendEOI_PIC1(struct InterruptDriver* self);
void InterruptDriver_SendEOI_PIC2(struct InterruptDriver* self);
void InterruptDriver_SetMask_PIC1(struct InterruptDriver* self, uint8_t mask);
void InterruptDriver_SetMask_PIC2(struct InterruptDriver* self, uint8_t mask);
uint8_t Interrupt_ReadISR_PIC1(struct InterruptDriver* self);
uint8_t Interrupt_ReadISR_PIC2(struct InterruptDriver* self);
uint8_t Interrupt_ReadIRR_PIC1(struct InterruptDriver* self);
uint8_t Interrupt_ReadIRR_PIC2(struct InterruptDriver* self);

void InterruptDriver_SetupDescriptorTable();
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