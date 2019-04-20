#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "FloppyConstants.h"

struct Kernel;

struct FloppyDriver
{
    struct Kernel* Kernel;
    enum FloppyStatus Master;
    enum FloppyStatus Slave;
    volatile int MotorTicks[2];
    volatile enum FloppyMotorState MotorState[2];
    volatile bool InterruptWaiting;
};

bool FloppyDriver_Constructor(struct FloppyDriver* self, struct Kernel* kernel);

void FloppyDriver_WriteCommand(struct FloppyDriver* self, enum FloppyCommand cmd);
uint8_t FloppyDriver_ReadData(struct FloppyDriver* self);
void FloppyDriver_CheckInterrupt(struct FloppyDriver* self, int *st0, int *cyl);
int32_t FloppyDriver_Calibrate(struct FloppyDriver* self);
int32_t FloppyDriver_Reset(struct FloppyDriver* self);
void FloppyDriver_IRQWait(struct FloppyDriver* self);
void FloppyDriver_Motor(struct FloppyDriver* self, int onoff);
void FloppyDriver_MotorKill(struct FloppyDriver* self);
void FloppyDriver_Timer(struct FloppyDriver* self);
int FloppyDriver_Seek(struct FloppyDriver* self, unsigned cyli, int head);
static void FloppyDriver_DMAInit(struct FloppyDriver* self, enum FloppyDirection dir);
int32_t FloppyDriver_DoTrack(struct FloppyDriver* self, uint32_t cyl, enum FloppyDirection dir);
int32_t FloppyDriver_ReadTrack(struct FloppyDriver* self, uint32_t cyl);
int32_t FloppyDriver_WriteTrack(struct FloppyDriver* self, uint32_t cyl);

void FloppyDriver_InterruptHandler();

bool FloppyDriver_CheckBusy(struct FloppyDriver* self);
enum FloppyStatus FloppyDriver_GetStatus(uint8_t data);

bool FloppyDriver_MasterPresent(struct FloppyDriver* self);
bool FloppyDriver_SlavePresent(struct FloppyDriver* self);

uint8_t FloppyDriver_ReadRegisterStatusA(struct FloppyDriver* self);
uint8_t FloppyDriver_ReadRegisterStatusB(struct FloppyDriver* self);

uint8_t FloppyDriver_ReadRegisterDigitalOutput(struct FloppyDriver* self);
void    FloppyDriver_WriteRegisterDigitalOutput(struct FloppyDriver* self, uint8_t data);

uint8_t FloppyDriver_ReadRegisterTapeDrive(struct FloppyDriver* self);
void    FloppyDriver_WriteRegisterTapeDrive(struct FloppyDriver* self, uint8_t data);

uint8_t FloppyDriver_ReadRegisterMainStatus(struct FloppyDriver* self);
void    FloppyDriver_WriteRegisterDataRateSelect(struct FloppyDriver* self, uint8_t data);

uint8_t FloppyDriver_ReadRegisterDataFIFO(struct FloppyDriver* self);
void    FloppyDriver_WriteRegisterDataFIFO(struct FloppyDriver* self, uint8_t data);

uint8_t FloppyDriver_ReadRegisterDigitalInput(struct FloppyDriver* self);

void FloppyDriver_WriteRegisterConfigurationControl(struct FloppyDriver* self, uint8_t data);