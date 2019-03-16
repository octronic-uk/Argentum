#include "Keyboard.h"
#include <Kernel.h>
#include <Devices/IO/IO.h>
#include <Devices/Interrupt/Interrupt.h>
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>


void Keyboard_Constructor()
{
	KeyboardBufferLocation = 0;
	memset(KeyboardBuffer,0,sizeof(KeyboardEventData)*KEYBOARD_BUFFER_SIZE);
	printf("Setting Keyboard IDT entry.");
	unsigned long keyboard_address;
	keyboard_address = (unsigned long)Keyboard_OnInterrupt;
	IDTEntry.mOffsetLowerBits = keyboard_address & 0xffff;
	IDTEntry.mSelector = KERNEL_CODE_SEGMENT_OFFSET;
	IDTEntry.mZero = 0;
	IDTEntry.mTypeAttribute = INTERRUPT_GATE;
	IDTEntry.mOffsetHigherBits = (keyboard_address & 0xffff0000) >> 16;
	Interrupt_SetIDTEntry(0x21,IDTEntry);
}

void Keyboard_OnInterrupt()
{
	unsigned char status;
	char keycode;

	/* write EOI */
	IO_WritePort(0x20, 0x20);

	status = IO_ReadPort(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01)
	{
		keycode = IO_ReadPort(KEYBOARD_DATA_PORT);
		if(keycode < 0)
		{
			return;
		}
		if (KeyboardBufferLocation < KEYBOARD_BUFFER_SIZE)
		{
			KeyboardBuffer[KeyboardBufferLocation].mKeycode = keycode;
			KeyboardBuffer[KeyboardBufferLocation].mStatus = status;
			KeyboardBufferLocation++;
		}
		else
		{
			Keyboard_BufferOverflow();
		}
	}
	Keyboard_HandleEvents();
}

void Keyboard_HandleEvents()
{
	int i;
	for (i=0;i<KeyboardBufferLocation; i++)
	{
		switch (KeyboardBuffer[i].mKeycode)
		{
			case KEY_UP:
				//Screen_MoveScrollOffset(-1);
				break;
			case KEY_DOWN:
				//Screen_MoveScrollOffset(1);
				break;
			default:
				break;
		}
	}
	KeyboardBufferLocation = 0;
}

void Keyboard_BufferOverflow()
{
	printf("ERR: Keyboard Buffer Overflow");
}

void Keyboard_IRQInit()
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	printf("Enabline IRQ1 [ONLY] for Keyboard");
	IO_WritePort(0x21 , 0xFD);
}
