#include <Drivers/PIT/PITDriver.h>

#include <stdio.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Interrupt/InterruptDriver.h>

extern Kernel _Kernel;

bool PITDriver_Constructor(PITDriver* self)
{
    printf("PIT Driver: Constructing\n");
    self->Ticks = 0;
    PITDriver_SetInterruptHandlerFunction(self);
    return true;
}

void PITDriver_IncrementTicks(PITDriver* self)
{
    self->Ticks++;
}

void PITDriver_ResetTicks(PITDriver* self)
{
    self->Ticks = 0;
}


void PITDriver_SetInterruptHandlerFunction(PITDriver* self)
{
    if (self->Debug)
    {
        printf("PIT: Setting interrupt function\n");
    }
    InterruptDriver_SetHandlerFunction(&_Kernel.Interrupt, 0, PITDriver_InterruptHandler);
}

void PITDriver_InterruptHandler()
{
    PITDriver* self = &_Kernel.PIT;
    PITDriver_IncrementTicks(self);
}