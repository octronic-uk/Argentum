#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "Scancode.h"

enum KeyboardState
{
    KEY_STATE_PRESSED,
    KEY_STATE_RELEASED,
    KEY_STATE_NONE
};

struct KeyboardAction
{
    uint8_t Scancode;
    enum KeyboardState KeyState;
    bool Valid;
};

static struct KeyboardAction KA_NO_ACTION = 
{
    .Scancode = SCANCODE_NONE,
    .KeyState = KEY_STATE_NONE
};