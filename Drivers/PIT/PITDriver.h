#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "PITConstants.h"

struct PITDriver
{
    bool Debug;
    uint64_t Ticks;
};
typedef struct PITDriver PITDriver;

bool PITDriver_Constructor(PITDriver* self);
void PITDriver_IncrementTicks(PITDriver* self);
void PITDriver_SetInterruptHandlerFunction(PITDriver* self);
void PITDriver_ResetTicks(PITDriver* self);
void PITDriver_InterruptHandler();