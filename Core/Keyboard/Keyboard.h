#pragma once
#include "../Types.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 16

extern void tkKeyboard_HandlerASM();

typedef struct 
{
    uint8_t mKeycode;
    uint8_t mStatus;
} tkKeyboard_EventData;

static tkKeyboard_EventData KeyboardBuffer[KEYBOARD_BUFFER_SIZE];
static unsigned char KeyboardBufferLocation = 0;

void tkKeyboard_Init();
void tkKeyboard_SetupIDT();
void tkKeyboard_Handler();
void tkKeyboard_IRQInit();
void tkKeyboard_BufferOverflow();
void tkKeyboard_HandleEvents();

// Keyboard Dey Definitions
#define KEY_UP 0x48
#define KEY_DOWN 0x50
