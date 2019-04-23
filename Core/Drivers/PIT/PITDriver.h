#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "PITConstants.h"

struct PITDriver
{
    bool Debug;
    uint64_t Ticks;
};

bool PITDriver_Constructor(struct PITDriver* self);
void PITDriver_IncrementTicks(struct PITDriver* self);
void PITDriver_SetInterruptHandlerFunction(struct PITDriver* self);
void PITDriver_ResetTicks(struct PITDriver* self);
void PITDriver_InterruptHandler();