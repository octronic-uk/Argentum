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

typedef struct FloppyDriver FloppyDriver;

struct CylinderHeadSector
{
    uint32_t Cylinder;
    uint32_t Head;
    uint32_t Sector;
};

typedef struct CylinderHeadSector CylinderHeadSector;

bool FloppyDriver_Constructor(FloppyDriver* self);

void FloppyDriver_WriteCommand(FloppyDriver* self, enum FloppyCommand cmd);
uint8_t FloppyDriver_ReadData(FloppyDriver* self);
void FloppyDriver_CheckInterrupt(FloppyDriver* self, int *st0, int *cyl);
bool FloppyDriver_Calibrate(FloppyDriver* self);
bool FloppyDriver_Reset(FloppyDriver* self);
void FloppyDriver_IRQWait(FloppyDriver* self);
void FloppyDriver_Motor(FloppyDriver* self, int onoff);
void FloppyDriver_MotorKill(FloppyDriver* self);
void FloppyDriver_Timer(FloppyDriver* self);
int FloppyDriver_Seek(FloppyDriver* self, unsigned cyli, int head);
static void FloppyDriver_DMAInit(FloppyDriver* self, enum FloppyDirection dir);

bool FloppyDriver_DoSector(FloppyDriver* self, struct CylinderHeadSector cyl, enum FloppyDirection dir);
bool FloppyDriver_ReadSectorLBA(FloppyDriver* self, uint32_t lba, uint8_t* buffer);
bool FloppyDriver_ReadSector(FloppyDriver* self, CylinderHeadSector cyl);
bool FloppyDriver_WriteSectorLBA(FloppyDriver* self, uint32_t lba, uint8_t* buffer);
bool FloppyDriver_WriteSector(FloppyDriver* self, CylinderHeadSector cyl);

void FloppyDriver_InterruptHandler();

enum FloppyType FloppyDriver_GetType(uint8_t data);

bool FloppyDriver_MasterPresent(FloppyDriver* self);
bool FloppyDriver_SlavePresent(FloppyDriver* self);

uint8_t FloppyDriver_ReadRegisterStatusA(FloppyDriver* self);
uint8_t FloppyDriver_ReadRegisterStatusB(FloppyDriver* self);

uint8_t FloppyDriver_ReadRegisterDigitalOutput(FloppyDriver* self);
void    FloppyDriver_WriteRegisterDigitalOutput(FloppyDriver* self, uint8_t data);

uint8_t FloppyDriver_ReadRegisterTapeDrive(FloppyDriver* self);
void    FloppyDriver_WriteRegisterTapeDrive(FloppyDriver* self, uint8_t data);

uint8_t FloppyDriver_ReadRegisterMainStatus(FloppyDriver* self);
void    FloppyDriver_WriteRegisterDataRateSelect(FloppyDriver* self, uint8_t data);

uint8_t FloppyDriver_ReadRegisterDataFIFO(FloppyDriver* self);
void    FloppyDriver_WriteRegisterDataFIFO(FloppyDriver* self, uint8_t data);

uint8_t FloppyDriver_ReadRegisterDigitalInput(FloppyDriver* self);

void FloppyDriver_WriteRegisterConfigurationControl(FloppyDriver* self, uint8_t data);

void FloppyDriver_DebugDMABuffer(FloppyDriver* self);
struct CylinderHeadSector FloppyDriver_LbaToChs(uint32_t lba);

uint8_t* FloppyDriver_GetDMABuffer(FloppyDriver* self);