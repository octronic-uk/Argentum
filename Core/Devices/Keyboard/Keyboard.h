#pragma once

// Keyboard Dey Definitions
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 16
#define KEY_UP 0x48
#define KEY_DOWN 0x50

#include <LibC/include/stdint.h>
#include <Devices/Interrupt/Interrupt.h>

extern void tkKeyboardHandlerASM();

typedef struct
{
	uint8_t mKeycode;
	uint8_t mStatus;
} KeyboardEventData;

KeyboardEventData KeyboardBuffer[KEYBOARD_BUFFER_SIZE];
uint8_t KeyboardBufferLocation;
InterruptDescriptorTableEntry IDTEntry;

void Keyboard_Constructor();
void Keyboard_Handler();
void Keyboard_IRQInit();
void Keyboard_BufferOverflow();
void Keyboard_HandleEvents();
void Keyboard_OnInterrupt();

