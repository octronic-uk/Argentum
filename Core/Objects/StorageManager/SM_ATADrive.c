#include "SM_ATADrive.h"

#include <stdio.h>
#include <Kernel.h>
#include <Drivers/ATA/ATATypes.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include "FAT16/FatVolume.h"
#include "MBR/MBRPartitionEntry.h"
#include "SMVolume.h"

extern struct Kernel _Kernel;

bool SM_ATADrive_Constructor(struct SM_ATADrive* self, uint8_t ata_device_id)
{
    printf("SM_ATADrive: Constructing drive %d\n",ata_device_id);
    self->Exists = true;
    self->Debug = false;
    self->AtaIndex = ata_device_id;

    if (self->Debug) 
    {
        PS2Driver_WaitForKeyPress("SMDisk: Reading MBR");
    }
    MBR_Constructor(&self->MasterBootRecord, &_Kernel.ATA, self->AtaIndex);

    uint8_t volume_id;
    for (volume_id = 0; volume_id < SM_DRIVE_MAX_VOLUMES; volume_id++)
    {
        struct MBRPartitionEntry* pe = SM_ATADrive_GetMBRPartitionEntry(&self->MasterBootRecord, volume_id);
        if (pe->Status == 0x80)// || pe->Status == 0x00)
        {
            self->VolumeExists[volume_id] = true;
            SMVolume_Constructor(&self->Volumes[volume_id], self, volume_id, pe->LBAFirstSector, pe->SectorsInPartition);
        }
    }
}

struct MBRPartitionEntry* SM_ATADrive_GetMBRPartitionEntry(struct MBR* mbr, uint8_t volume)
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

void SM_ATADrive_ListVolumes(struct SM_ATADrive* self)
{
    printf("SM_ATADrive: Listing volumes on ata%d\n",self->AtaIndex);
    uint8_t i;
    for(i=0; i<SM_DRIVE_MAX_VOLUMES; i++)
    {
        if (self->VolumeExists[i])
        {
            printf("\t* ata%d%c%d://\n",self->AtaIndex,SM_PATH_VOLUME_DELIMETER,i);
        }
    }
}

struct SMVolume* SM_ATADrive_GetVolume(struct SM_ATADrive* self, uint8_t volume_id)
{
    if (self->VolumeExists[volume_id])
    {
        return &self->Volumes[volume_id];
    }
    else
    {
        return 0;
    }
}

