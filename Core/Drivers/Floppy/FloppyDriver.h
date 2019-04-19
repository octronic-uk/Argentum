#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "FloppyConstants.h"

enum FloppyStatus
{
    FLOPPY_STATUS_NO_DRIVE,
    FLOPPY_STATUS_525_360,
    FLOPPY_STATUS_525_12,
    FLOPPY_STATUS_35_720,
    FLOPPY_STATUS_35_144,
    FLOPPY_STATUS_35_288,
};

struct Kernel;

struct FloppyDriver
{
    struct Kernel* Kernel;
    enum FloppyStatus Master;
    enum FloppyStatus Slave;
};

bool FloppyDriver_Constructor(struct FloppyDriver* self, struct Kernel* kernel);
void FloppyDriver_Lba2Chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector);

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

bool FloppyDriver_CheckBusy(struct FloppyDriver* self);
enum FloppyStatus FloppyDriver_GetStatus(uint8_t data);