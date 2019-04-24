#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "FloppyConstants.h"

struct FloppyDriver
{
    bool Debug;
    bool DebugIO;
    enum FloppyType Master;
    enum FloppyType Slave;
    volatile int MotorTicks[2];
    volatile enum FloppyMotorState MotorState[2];
    volatile bool InterruptWaiting;
};

struct CylinderHeadSector
{
    uint32_t Cylinder;
    uint32_t Head;
    uint32_t Sector;
};

bool FloppyDriver_Constructor(struct FloppyDriver* self);

void FloppyDriver_WriteCommand(struct FloppyDriver* self, enum FloppyCommand cmd);
uint8_t FloppyDriver_ReadData(struct FloppyDriver* self);
void FloppyDriver_CheckInterrupt(struct FloppyDriver* self, int *st0, int *cyl);
bool FloppyDriver_Calibrate(struct FloppyDriver* self);
bool FloppyDriver_Reset(struct FloppyDriver* self);
void FloppyDriver_IRQWait(struct FloppyDriver* self);
void FloppyDriver_Motor(struct FloppyDriver* self, int onoff);
void FloppyDriver_MotorKill(struct FloppyDriver* self);
void FloppyDriver_Timer(struct FloppyDriver* self);
int FloppyDriver_Seek(struct FloppyDriver* self, unsigned cyli, int head);
static void FloppyDriver_DMAInit(struct FloppyDriver* self, enum FloppyDirection dir);

bool FloppyDriver_DoSector(struct FloppyDriver* self, struct CylinderHeadSector cyl, enum FloppyDirection dir);
bool FloppyDriver_ReadSectorLBA(struct FloppyDriver* self, uint32_t lba);
bool FloppyDriver_ReadSector(struct FloppyDriver* self, struct CylinderHeadSector cyl);
bool FloppyDriver_WriteSectorLBA(struct FloppyDriver* self, uint32_t lba);
bool FloppyDriver_WriteSector(struct FloppyDriver* self, struct CylinderHeadSector cyl);

void FloppyDriver_InterruptHandler();

enum FloppyType FloppyDriver_GetType(uint8_t data);

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

void FloppyDriver_DebugDMABuffer(struct FloppyDriver* self);
struct CylinderHeadSector FloppyDriver_LbaToChs(uint32_t lba);