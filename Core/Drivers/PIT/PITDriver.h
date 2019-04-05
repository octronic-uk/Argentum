#pragma once

#include <stdint.h>
#include <stdbool.h>

struct Kernel;

struct PITDriver
{
    struct Kernel* Kernel;
    bool Debug;
    uint64_t Ticks;
};

bool PITDriver_Constructor(struct PITDriver* self, struct Kernel* kernel);
void PITDriver_IncrementTicks(struct PITDriver* self);
void PITDriver_SetInterruptHandlerFunction(struct PITDriver* self);
void PITDriver_ResetTicks(struct PITDriver* self);
void PITDriver_InterruptHandler();