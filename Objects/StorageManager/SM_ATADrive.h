#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "SMVolume.h"
#include "../MBR/MBR.h"

#define SM_DRIVE_MAX_VOLUMES 4

struct ATA_IDEDevice;
struct MBRPartitionEntry;
struct SMDirectoryEntry;

struct SM_ATADrive
{
    bool Debug;
    bool Exists;
	struct MBR MasterBootRecord;
    struct SMVolume Volumes[SM_DRIVE_MAX_VOLUMES];
    bool VolumeExists[SM_DRIVE_MAX_VOLUMES];
    uint8_t AtaIndex;
};

bool SM_ATADrive_Constructor(struct SM_ATADrive* self, uint8_t ide_device_id);

struct MBRPartitionEntry* SM_ATADrive_GetMBRPartitionEntry(struct MBR* mbr, uint8_t partition);
void SM_ATADrive_ListVolumes(struct SM_ATADrive* self);
struct SMVolume* SM_ATADrive_GetVolume(struct SM_ATADrive* self, uint8_t volume_id); 