#pragma once
#include "../Types.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_BUFFER_SIZE 16

extern void tkKeyboardHandlerASM();

typedef struct 
{
    uint8_t mKeycode;
    uint8_t mStatus;
} tkKeyboardEventData;

static tkKeyboardEventData KeyboardBuffer[KEYBOARD_BUFFER_SIZE];
static unsigned char KeyboardBufferLocation = 0;

void tkKeyboardInit();
void tkKeyboardSetupIDT();
void tkKeyboardHandler();
void tkKeyboardIRQInit();
void tkKeyboardBufferOverflow();
void tkKeyboardHandleEvents();

// Keyboard Dey Definitions
#define KEY_UP 0x48
#define KEY_DOWN 0x50
