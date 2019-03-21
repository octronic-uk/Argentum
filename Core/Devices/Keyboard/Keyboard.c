#include "Keyboard.h"
#include <Kernel.h>
#include <Devices/IO/IO.h>
#include <Devices/Screen/Screen.h>
#include <Devices/Interrupt/Interrupt.h>
#include <string.h>
#include <stdio.h>

uint8_t Keyboard_Debug;

Keyboard_Event Keyboard_EventBuffer[KEYBOARD_EVENT_BUFFER_SIZE];
uint8_t Keyboard_EventBufferIndex;
Interrupt_DescriptorTableEntry Keyboard_IDTEntry;

void Keyboard_SetDebug(uint8_t debug)
{
	Keyboard_Debug = debug;
}

uint8_t Keyboard_GetDebug()
{
	return Keyboard_Debug;
}

void Keyboard_Constructor()
{
	Keyboard_EventBufferIndex = 0;
	memset(Keyboard_EventBuffer,0,sizeof(Keyboard_Event)*KEYBOARD_EVENT_BUFFER_SIZE);
	if (Keyboard_Debug){
	printf("Setting Keyboard IDT entry.\n");
	}
	uint32_t keyboard_asm = (uint32_t)Keyboard_EventHandlerASM;
	Keyboard_IDTEntry.mOffsetLowerBits = keyboard_asm & 0xffff;
	Keyboard_IDTEntry.mOffsetHigherBits = (keyboard_asm & 0xffff0000) >> 16;
	Keyboard_IDTEntry.mSelector = INTERRUPT_KERNEL_CODE_SEGMENT_OFFSET;
	Keyboard_IDTEntry.mZero = 0;
	Keyboard_IDTEntry.mTypeAttribute = INTERRUPT_GATE;
	Interrupt_SetIDTEntry(0x21,Keyboard_IDTEntry);
}

void Keyboard_OnInterrupt()
{
	unsigned char status;
	char keycode;

	/* write EOI */
	uint8_t debug_prev = Interrupt_GetDebug();
	Interrupt_SetDebug(0);
	Interrupt_SendEOI_PIC1();

	status = IO_ReadPort8b(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01)
	{
		keycode = IO_ReadPort8b(KEYBOARD_DATA_PORT);
		if(keycode < 0)
		{
			return;
		}
		if (Keyboard_EventBufferIndex < KEYBOARD_EVENT_BUFFER_SIZE-1)
		{
			Keyboard_EventBuffer[Keyboard_EventBufferIndex].mKeycode = keycode;
			Keyboard_EventBuffer[Keyboard_EventBufferIndex].mStatus = status;
			if (Keyboard_Debug){
			printf("Keyboard: Pushed Event %d\n",Keyboard_EventBufferIndex);
			}
			Keyboard_EventBufferIndex++;
		}
		else
		{
			printf("ERROR: Keyboard Buffer Overflow\n");
		}
	}
	Keyboard_HandleEvents(); // TODO - Move into a Task
	Interrupt_SetDebug(debug_prev);
}

void Keyboard_HandleEvents()
{
	int i;
	for (i=0; i<=Keyboard_EventBufferIndex; i++)
	{
		switch (Keyboard_EventBuffer[i].mKeycode)
		{
			case KEY_UP:
				Screen_MoveScrollOffset(-1);
				break;
			case KEY_DOWN:
				Screen_MoveScrollOffset(1);
				break;
			default:
				break;
		}
	}
	Keyboard_EventBufferIndex = 0;
}