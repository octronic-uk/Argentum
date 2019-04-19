#include <Drivers/PIT/PITDriver.h>

#include <stdio.h>

#include <Kernel.h>
#include <Drivers/Interrupt/InterruptDriver.h>

static struct Kernel* _Kernel;

bool PITDriver_Constructor(struct PITDriver* self, struct Kernel* kernel)
{
    printf("PIT: Constructing\n");
    self->Kernel = kernel;
    _Kernel = kernel;
    self->Ticks = 0;
    PITDriver_SetInterruptHandlerFunction(self);
    return true;
}

void PITDriver_IncrementTicks(struct PITDriver* self)
{
    self->Ticks++;
}

void PITDriver_ResetTicks(struct PITDriver* self)
{
    self->Ticks = 0;
}


void PITDriver_SetInterruptHandlerFunction(struct PITDriver* self)
{
    if (self->Debug)
    {
        printf("PIT: Setting interrupt function\n");
    }
    InterruptDriver_SetHandlerFunction(&self->Kernel->Interrupt, 0, PITDriver_InterruptHandler);
}

void PITDriver_InterruptHandler()
{
    struct PITDriver* self = &_Kernel->PIT;
    PITDriver_IncrementTicks(self);
}