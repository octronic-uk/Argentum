#pragma once

#include <stdbool.h>
#include <stdint.h>

struct KeyboardEvent
{
    bool Pressed;
    uint8_t Character;
    uint32_t Scancode;
};
typedef struct KeyboardEvent KeyboardEvent;

struct MouseEvent
{
    uint32_t X;
    uint32_t Y;
    bool Button[3];
};

typedef struct MouseEvent MouseEvent;