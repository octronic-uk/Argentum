#include "SMDrive.h"

#include <stdio.h>
#include <Kernel.h>
#include <Drivers/ATA/ATATypes.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include "FAT/FatVolume.h"
#include "MBR/MBRPartitionEntry.h"
#include "SMVolume.h"

bool SMDrive_Constructor(struct SMDrive* self, struct Kernel* kernel, uint8_t ata_device_id)
{
    printf("SMDrive: Constructing drive %d\n",ata_device_id);
    self->Debug = false;
    self->Kernel = kernel;
    self->AtaIndex = ata_device_id;

    if (self->Debug) 
    {
        printf("SMDisk: Reading MBR\n");
        PS2Driver_WaitForKeyPress();
    }
    MBR_Constructor(&self->MasterBootRecord, &self->Kernel->ATA, self->AtaIndex);

    uint8_t volume_id;
    for (volume_id = 0; volume_id < SM_DRIVE_MAX_VOLUMES; volume_id++)
    {
        struct MBRPartitionEntry* pe = SMDrive_GetMBRPartitionEntry(&self->MasterBootRecord, volume_id);
        if (pe->Status == 0x80)// || pe->Status == 0x00)
        {
            self->VolumeExists[volume_id] = true;
            SMVolume_Constructor(&self->Volumes[volume_id], self, self->Kernel, volume_id,
                pe->LBAFirstSector, pe->SectorsInPartition
            );
        }
    }
}

void SMDrive_Destructor(struct SMDrive* self)
{

}

struct MBRPartitionEntry* SMDrive_GetMBRPartitionEntry(struct MBR* mbr, uint8_t volume)
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

void SMDrive_ListVolumes(struct SMDrive* self)
{
    printf("SMDrive: Listing volumes on drive%d\n",self->AtaIndex);
    uint8_t i;
    for(i=0; i<SM_DRIVE_MAX_VOLUMES; i++)
    {
        if (self->VolumeExists[i])
        {
            printf("volume%d\n",i);
        }
    }
}

struct SMVolume* SMDrive_GetVolume(struct SMDrive* self, uint8_t volume_id)
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

