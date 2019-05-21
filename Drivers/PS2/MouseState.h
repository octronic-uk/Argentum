#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "Event.h"

#define MOUSE_STATE_DATA_BUFFER_MAX 3

struct MouseState
{
    bool    Debug;
    // Positions
    volatile int32_t PositionX;
    int32_t PositionXMax;
    volatile int32_t PositionY;
    int32_t PositionYMax;
    // Buttons
    uint8_t NumButtons;
    volatile bool    Button[3];
    // Buffer
    uint8_t DataBuffer[3];
    uint8_t DataBufferIndex;
    void(*MouseEventCallback)(MouseEvent);
};

typedef struct MouseState MouseState;

bool MouseState_Constructor(MouseState* self);
void MouseState_OnData(MouseState* self, uint8_t data);
void MouseState_ParseDataBuffer(MouseState* self);
void MouseState_Debug(MouseState* self);

void MouseState_SetEventCallback(MouseState* self, void(*callback)(MouseEvent));