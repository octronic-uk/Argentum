#include "SMDrive.h"

#include <stdio.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/ATA/ATATypes.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include <Objects/FAT16/FatVolume.h>
#include <Objects/MBR/MBRPartitionEntry.h>
#include <Objects/StorageManager/SMDrive.h>

extern struct Kernel _Kernel;

bool SMDrive_ATAConstructor(struct SMDrive* self, uint8_t ata_device_id)
{
    printf("SMDrive: ATA Constructing drive %d\n",ata_device_id);
    self->Exists = true;
    self->Debug = false;
    self->AtaIndex = ata_device_id;

    if (self->Debug) 
    {
        PS2Driver_WaitForKeyPress("SMDrive: Reading MBR");
    }

    MBR_Constructor(&self->MasterBootRecord, &_Kernel.ATA, self->AtaIndex);

    uint8_t volume_id;
    for (volume_id = 0; volume_id < SM_DRIVE_MAX_VOLUMES; volume_id++)
    {
        struct MBRPartitionEntry* pe = SMDrive_ATAGetMBRPartitionEntry(&self->MasterBootRecord, volume_id);
        if (pe->Status == 0x80)// || pe->Status == 0x00)
        {
            SMVolume_ATAConstructor(&self->Volumes[volume_id], self, volume_id, pe->LBAFirstSector, pe->SectorsInPartition);
        }
    }
}

bool SMDrive_FloppyConstructor(struct SMDrive* self, uint8_t floppy_drive_id)
{
    printf("SMDrive: Floppy Constructing drive %d\n", floppy_drive_id);
    self->Debug = true;
    self->FloppyIndex = floppy_drive_id;
    self->Exists = true;
    SMVolume_FloppyConstructor(&self->Volumes[0],self, self->FloppyIndex);
} 

struct MBRPartitionEntry* SMDrive_ATAGetMBRPartitionEntry(struct MBR* mbr, uint8_t volume)
{
    // Read the MBR to find out where the volume is located 
    switch (volume)
    {
        case 0:
            return &mbr->Record.PartitionEntry1;
        case 1:
            return &mbr->Record.PartitionEntry2;
        case 2:
            return &mbr->Record.PartitionEntry3;
        case 3:
            return &mbr->Record.PartitionEntry4;
        default:
            return 0;
    }
}

void SMDrive_ATAListVolumes(struct SMDrive* self)
{
    printf("SM_ATADrive: Listing volumes on ata%d\n",self->AtaIndex);
    uint8_t i;
    for(i=0; i<SM_DRIVE_MAX_VOLUMES; i++)
    {
        if (self->Volumes[i].Exists)
        {
            printf("\t* ata%d%c%d://\n",self->AtaIndex, SM_PATH_VOLUME_DELIMETER,i);
        }
    }
}

struct SMVolume* SMDrive_GetVolume(struct SMDrive* self, uint8_t volume_id)
{
    if (self->Volumes[volume_id].Exists)
    {
        return &self->Volumes[volume_id];
    }
    else
    {
        return 0;
    }
}

