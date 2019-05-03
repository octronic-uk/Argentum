#pragma once
#include <stdint.h>
#include <stdbool.h>

#include "KeyboardAction.h"

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
    enum ScancodeParserState State;
    volatile struct KeyboardAction CurrentAction;
    volatile bool ActionValid;
    bool Shift;
    bool Alt;
    bool Ctrl;
};

bool ScancodeParser_Constructor(struct ScancodeParser* self);
void ScancodeParser_ParseScancode(struct ScancodeParser* self, uint8_t byte);
void ScancodeParser_ClearCurrentAction(struct ScancodeParser* self);
void ScancodeParser_WaitForKeyPressAction(struct ScancodeParser* self);
int32_t ScancodeParser_GetChar(struct ScancodeParser* self);