#include "FloppyDriver.h"

#include <string.h>
#include <stdio.h>
#include <Drivers/CMOS/CMOSDriver.h>
#include <Drivers/IO/IODriver.h>

bool FloppyDriver_Constructor(struct FloppyDriver* self, struct Kernel* kernel)
{
    printf("Floppy: Constructing\n");
    memset(self,0,sizeof(struct FloppyDriver));
    self->Kernel = kernel;
    uint8_t cmosConfig = CMOSDriver_ReadFloppyConfiguration(true);
    printf("Floppy: CMOS Configuration 0x%x\n",cmosConfig);
    uint8_t master, slave;
    master = (cmosConfig & 0xF0) >> 4;
    slave  = (cmosConfig & 0x0F);

    self->Master = FloppyDriver_GetStatus(master);
    self->Slave = FloppyDriver_GetStatus(slave);

    return true;
}

void FloppyDriver_Lba2Chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
{
    *cyl    = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
    *head   = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK);
    *sector = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1);
}

uint8_t FloppyDriver_ReadRegisterStatusA(struct FloppyDriver* self) 
{
    return IO_ReadPort8b(FLOPPY_REGISTER_STATUS_A);
}

uint8_t FloppyDriver_ReadRegisterStatusB(struct FloppyDriver* self) 
{
    return IO_ReadPort8b(FLOPPY_REGISTER_STATUS_B);
}

uint8_t FloppyDriver_ReadRegisterDigitalOutput(struct FloppyDriver* self)
{
    return IO_ReadPort8b(FLOPPY_REGISTER_DIGITAL_OUTPUT);
}

void FloppyDriver_WriteRegisterDigitalOutput(struct FloppyDriver* self, uint8_t data)
{
    IO_WritePort8b(FLOPPY_REGISTER_DIGITAL_OUTPUT, data);
}

uint8_t FloppyDriver_ReadRegisterTapeDrive(struct FloppyDriver* self)
{
    return IO_ReadPort8b(FLOPPY_REGISTER_TAPE_DRIVE);
}

void FloppyDriver_WriteRegisterTapeDrive(struct FloppyDriver* self, uint8_t data)
{
    IO_WritePort8b(FLOPPY_REGISTER_TAPE_DRIVE, data);
}

uint8_t FloppyDriver_ReadRegisterMainStatus(struct FloppyDriver* self)
{
    return IO_ReadPort8b(FLOPPY_REGISTER_MAIN_STATUS);
}

void FloppyDriver_WriteRegisterDataRateSelect(struct FloppyDriver* self, uint8_t data)
{
    IO_WritePort8b(FLOPPY_REGISTER_DATARATE_SELECT,data);
}

uint8_t FloppyDriver_ReadRegisterDataFIFO(struct FloppyDriver* self)
{
    return IO_ReadPort8b(FLOPPY_REGISTER_DATA_FIFO);
}

void FloppyDriver_WriteRegisterDataFIFO(struct FloppyDriver* self, uint8_t data)
{
    IO_WritePort8b(FLOPPY_REGISTER_DATA_FIFO, data);
}

uint8_t FloppyDriver_ReadRegisterDigitalInput(struct FloppyDriver* self)
{
    return IO_ReadPort8b(FLOPPY_REGISTER_DIGITAL_INPUT);
}

void FloppyDriver_WriteRegisterConfigurationControl(struct FloppyDriver* self, uint8_t data)
{
    IO_WritePort8b(FLOPPY_REGISTER_CONFIGURATION_CONTROL, data);
}


bool FloppyDriver_CheckBusy(struct FloppyDriver* self)
{
    uint8_t status = FloppyDriver_ReadRegisterMainStatus(self);
    return (status & FLOPPY_MSR_CB) == 0 && 
           (status & FLOPPY_MSR_DIO) != 0;
}

enum FloppyStatus FloppyDriver_GetStatus(uint8_t data)
{
    switch (data)
    {
        case CMOS_FLOPPY_CONF_525_360:
            return FLOPPY_STATUS_525_360;

        case CMOS_FLOPPY_CONF_525_12:
            return FLOPPY_STATUS_525_12;

        case CMOS_FLOPPY_CONF_35_720:
            return FLOPPY_STATUS_35_720;

        case CMOS_FLOPPY_CONF_35_144:
            return FLOPPY_STATUS_35_144;

        case CMOS_FLOPPY_CONF_35_288:
            return FLOPPY_STATUS_35_288;

        case CMOS_FLOPPY_CONF_NO_DRIVE:
        default:
            return FLOPPY_STATUS_NO_DRIVE;
    }
}