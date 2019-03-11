#include "Keyboard.h"
#include "../Utilities.h"
#include "../Screen/Screen.h"
#include "../IO/IO.h"
#include "../Interrupt/Interrupt.h"

void tkKeyboard_Init()
{
	memset(KeyboardBuffer,0,sizeof(tkKeyboard_EventData)*KEYBOARD_BUFFER_SIZE);
}

void tkKeyboard_SetupIDT()
{
	tkScreen_PrintLine("Setting Keyboard IDT entry.");
    unsigned long keyboard_address;
	keyboard_address = (unsigned long)tkKeyboard_HandlerASM;
	InterruptDescriptorTable[0x21].mOffsetLowerBits = keyboard_address & 0xffff;
	InterruptDescriptorTable[0x21].mSelector = KERNEL_CODE_SEGMENT_OFFSET;
	InterruptDescriptorTable[0x21].mZero = 0;
	InterruptDescriptorTable[0x21].mTypeAttribute = INTERRUPT_GATE;
	InterruptDescriptorTable[0x21].mOffsetHigherBits = (keyboard_address & 0xffff0000) >> 16;
}

void tkKeyboard_Handler()
{
	unsigned char status;
	char keycode;

	/* write EOI */
	tkIO_WritePort(0x20, 0x20);

	status = tkIO_ReadPort(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) 
	{
		keycode = tkIO_ReadPort(KEYBOARD_DATA_PORT);
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
			tkKeyboard_BufferOverflow();
		}
	}
	tkKeyboard_HandleEvents();
}

void tkKeyboard_HandleEvents()
{	
	int i;
	for (i=0;i<KeyboardBufferLocation; i++)	
	{
		switch (KeyboardBuffer[i].mKeycode)
		{
			case KEY_UP:
				tkScreen_MoveScrollOffset(-1);
				break;
			case KEY_DOWN:
				tkScreen_MoveScrollOffset(1);
				break;
			default: 
				break;
		}
	}
	KeyboardBufferLocation = 0;
}

void tkKeyboard_BufferOverflow()
{
	tkScreen_PrintLine("ERR: Keyboard Buffer Overflow");
}

void tkKeyboard_IRQInit()
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	tkScreen_PrintLine("Enabline IRQ1 [ONLY] for Keyboard");
	tkIO_WritePort(0x21 , 0xFD);
}