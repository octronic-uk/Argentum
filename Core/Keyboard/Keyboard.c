#include "Keyboard.h"
#include "../Utilities.h"
#include "../Screen/Screen.h"
#include "../Interrupt/Interrupt.h"

void tkKeyboardInit()
{
	memset(KeyboardBuffer,0,sizeof(tkKeyboardEventData)*KEYBOARD_BUFFER_SIZE);
}

void tkKeyboardSetupIDT()
{
	tkScreenPrintLine("Setting Keyboard IDT entry.");
    unsigned long keyboard_address;
	keyboard_address = (unsigned long)tkKeyboardHandlerASM;
	InterruptDescriptorTable[0x21].mOffsetLowerBits = keyboard_address & 0xffff;
	InterruptDescriptorTable[0x21].mSelector = KERNEL_CODE_SEGMENT_OFFSET;
	InterruptDescriptorTable[0x21].mZero = 0;
	InterruptDescriptorTable[0x21].mTypeAttribute = INTERRUPT_GATE;
	InterruptDescriptorTable[0x21].mOffsetHigherBits = (keyboard_address & 0xffff0000) >> 16;
}

void tkKeyboardHandler()
{
	unsigned char status;
	char keycode;

	/* write EOI */
	tkInterruptWritePort(0x20, 0x20);

	status = tkInterruptReadPort(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) 
	{
		keycode = tkInterruptReadPort(KEYBOARD_DATA_PORT);
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
			tkKeyboardBufferOverflow();
		}
	}
	tkKeyboardHandleEvents();
}

void tkKeyboardHandleEvents()
{	
	int i;
	for (i=0;i<KeyboardBufferLocation; i++)	
	{
		switch (KeyboardBuffer[i].mKeycode)
		{
			case KEY_UP:
				tkScreenMoveScrollOffset(-1);
				break;
			case KEY_DOWN:
				tkScreenMoveScrollOffset(1);
				break;
			default: 
				break;
		}
	}
	KeyboardBufferLocation = 0;
}

void tkKeyboardBufferOverflow()
{
	tkScreenPrintLine("ERR: Keyboard Buffer Overflow");
}

void tkKeyboardIRQInit()
{
	/* 0xFD is 11111101 - enables only IRQ1 (keyboard)*/
	tkScreenPrintLine("Enabline IRQ1 [ONLY] for Keyboard");
	tkInterruptWritePort(0x21 , 0xFD);
}