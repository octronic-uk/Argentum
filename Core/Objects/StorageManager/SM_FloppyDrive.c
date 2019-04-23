#include "SM_FloppyDrive.h"

#include <stdio.h>
#include <Kernel.h>
#include <Drivers/ATA/ATATypes.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include "FAT16/FatVolume.h"
#include "MBR/MBRPartitionEntry.h"
#include "SMVolume.h"

extern struct Kernel _Kernel;

bool SM_FloppyDrive_Constructor(struct SM_FloppyDrive* self, uint8_t floppy_drive_id)
{
    printf("SM_FloppyDrive: Constructing drive %d\n", floppy_drive_id);
    self->Debug = false;
    self->DriveIndex = floppy_drive_id;

    if (self->Debug) 
    {
        PS2Driver_WaitForKeyPress("SM_FloppyDrive: Reading MBR");
    }
}