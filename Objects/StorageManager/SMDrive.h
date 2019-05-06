#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Objects/StorageManager/SMVolume.h>
#include <Objects/MBR/MBR.h>
#include <Objects/LinkedList/LinkedList.h>

#define SM_DRIVE_MAX_VOLUMES 4

struct ATA_IDEDevice;
struct MBRPartitionEntry;
struct SMDirectoryEntry;

enum SMDriveType
{
    SM_DRIVE_TYPE_NONE = 0,
    SM_DRIVE_TYPE_ATA,
    SM_DRIVE_TYPE_FLOPPY,
    SM_DRIVE_TYPE_RAMDISK
};

struct SMDrive
{
    bool Debug;
    bool Exists;
    //struct SMVolume Volumes[SM_DRIVE_MAX_VOLUMES];
    struct LinkedList Volumes;
    // ATA
	struct MBR MasterBootRecord;
    int8_t AtaIndex;
    // Floppy
    int8_t FloppyIndex;
    // Ram Disk
    int8_t RamDiskIndex;
};

bool SMDrive_ATAConstructor(struct SMDrive* self, uint8_t ata_device_index);
bool SMDrive_FloppyConstructor(struct SMDrive* self, uint8_t floppy_device_index);
bool SMDrive_RamDiskConstructor(struct SMDrive* self, uint8_t ram_disk_index);

void SMDrive_Destructor(struct SMDrive* self);
struct MBRPartitionEntry* SMDrive_ATAGetMBRPartitionEntry(struct MBR* mbr, uint8_t partition);
void SMDrive_ATAListVolumes(struct SMDrive* self);
struct SMVolume* SMDrive_GetVolume(struct SMDrive* self, uint8_t volume_id); 
enum SMDriveType SMDrive_GetDriveType(struct SMDrive* self);