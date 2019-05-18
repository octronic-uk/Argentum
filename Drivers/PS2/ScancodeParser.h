#pragma once
#include <stdint.h>
#include <stdbool.h>

#include "KeyboardAction.h"
#include "Event.h"

#define ACTION_BUFFER_SIZE 128 

enum ScancodeParserState
{
    STATE_START,
    STATE_GOT_EXTENDED,
    STATE_GOT_RELEASED,
    STATE_GOT_SCANCODE
};

struct ScancodeParser
{
    bool Debug;
    volatile enum ScancodeParserState State;
    volatile struct KeyboardAction CurrentAction;
    volatile bool ActionValid;
    volatile bool Shift;
    volatile bool Alt;
    volatile bool Ctrl;
    void(*KeyboardEventCallback)(KeyboardEvent); 
};

typedef struct ScancodeParser ScancodeParser;

bool ScancodeParser_Constructor(ScancodeParser* self);
void ScancodeParser_ParseScancode(ScancodeParser* self, uint8_t byte);
void ScancodeParser_ClearCurrentAction(ScancodeParser* self);
void ScancodeParser_WaitForKeyPressAction(ScancodeParser* self);
int32_t ScancodeParser_GetChar(ScancodeParser* self);

void ScancodeParser_SetEventCallback(ScancodeParser* self, void(*callback)(KeyboardEvent));