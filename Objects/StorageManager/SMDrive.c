#include "SMDrive.h"

#include <stdio.h>
#include <string.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/ATA/ATATypes.h>
#include <Drivers/ATA/ATADriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include <Objects/FAT/FatVolume.h>
#include <Objects/MBR/MBRPartitionEntry.h>
#include <Objects/StorageManager/SMDrive.h>
#include <Objects/RamDisk/RamDisk.h>

extern struct Kernel _Kernel;

bool SMDrive_ATAConstructor(struct SMDrive* self, uint8_t ata_device_id)
{
    printf("SMDrive: ATA Constructing drive %d\n",ata_device_id);
    memset(self,0,sizeof(struct SMDrive));
    LinkedList_Constructor(&self->Volumes);
    self->Exists = true;
    self->Debug = false;
    self->AtaIndex = ata_device_id;
    self->FloppyIndex = -1;
    self->RamDiskIndex = -1;

    MBR_Constructor(&self->MasterBootRecord, &_Kernel.ATA, self->AtaIndex);

    uint8_t volume_id;
    for (volume_id = 0; volume_id < SM_DRIVE_MAX_VOLUMES; volume_id++)
    {
        struct MBRPartitionEntry* pe = SMDrive_ATAGetMBRPartitionEntry(&self->MasterBootRecord, volume_id);
        if (pe->Status == 0x80)
        {
            struct SMVolume* vol = (struct SMVolume*)MemoryDriver_Allocate(&_Kernel.Memory,sizeof(struct SMVolume));
            SMVolume_ATAConstructor(vol, self, volume_id, pe->LBAFirstSector, pe->SectorsInPartition);
            LinkedList_PushBack(&self->Volumes,vol);
        }
    }
}

bool SMDrive_FloppyConstructor(struct SMDrive* self, uint8_t floppy_drive_id)
{
    printf("SMDrive: Floppy Constructing drive %d\n", floppy_drive_id);
    memset(self,0,sizeof(struct SMDrive));
    LinkedList_Constructor(&self->Volumes);
    self->Debug = true;
    self->AtaIndex = -1;
    self->FloppyIndex = floppy_drive_id;
    self->RamDiskIndex = -1;
    self->Exists = true;
    struct SMVolume* vol = (struct SMVolume*)MemoryDriver_Allocate(&_Kernel.Memory,sizeof(struct SMVolume));
    SMVolume_FloppyConstructor(vol, self, self->FloppyIndex);
    LinkedList_PushBack(&self->Volumes,vol);
} 

bool SMDrive_RamDiskConstructor(struct SMDrive* self, uint8_t ram_disk_id)
{
    printf("SMDrive: RamDisk Constructing drive %d\n", ram_disk_id);
    memset(self,0,sizeof(struct SMDrive));
    LinkedList_Constructor(&self->Volumes);
    self->Debug = true;
    self->AtaIndex = -1;
    self->FloppyIndex = -1;
    self->RamDiskIndex = ram_disk_id;
    self->Exists = true;
    struct SMVolume* vol = (struct SMVolume*)MemoryDriver_Allocate(&_Kernel.Memory,sizeof(struct SMVolume));
    SMVolume_RamDiskConstructor(vol, self, self->RamDiskIndex);
    LinkedList_PushBack(&self->Volumes,vol);
} 

void SMDrive_Destructor(struct SMDrive* self)
{
    uint8_t i, count;
    count = LinkedList_Size(&self->Volumes);
    for(i=0; i<count; i++)
    {
        struct SMVolume* vol = (struct SMVolume*)LinkedList_At(&self->Volumes,i);
        SMVolume_Destructor(vol);
    }
    LinkedList_FreeAllData(&self->Volumes);
    LinkedList_Destructor(&self->Volumes);
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
    uint8_t i, count;
    count = LinkedList_Size(&self->Volumes);
    for(i=0; i<count; i++)
    {
        struct SMVolume* vol = (struct SMVolume*)LinkedList_At(&self->Volumes,i);
        if (vol->Exists)
        {
            printf("\t* ata%d%c%d://\n",self->AtaIndex, SM_PATH_VOLUME_DELIMETER,i);
        }
    }
}

struct SMVolume* SMDrive_GetVolume(struct SMDrive* self, uint8_t volume_id)
{
    struct SMVolume* vol = (struct SMVolume*)LinkedList_At(&self->Volumes,volume_id);

    if (vol->Exists)
    {
        return vol;
    }
    else
    {
        return 0;
    }
}

enum SMDriveType SMDrive_GetDriveType(struct SMDrive* self)
{
    if (self->AtaIndex > -1)          return SM_DRIVE_TYPE_ATA;
    else if (self->FloppyIndex > -1)  return SM_DRIVE_TYPE_FLOPPY;
    else if (self->RamDiskIndex > -1) return SM_DRIVE_TYPE_RAMDISK;
    else                              return SM_DRIVE_TYPE_NONE;
}