#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "SMVolume.h"
#include "MBR/MBR.h"

#define SM_DRIVE_MAX_VOLUMES 4

struct Kernel;
struct ATA_IDEDevice;
struct MBRPartitionEntry;
struct SMDirectoryEntry;

struct SMDrive
{
    bool Debug;
    struct Kernel* Kernel;
	struct MBR MasterBootRecord;
    struct SMVolume Volumes[SM_DRIVE_MAX_VOLUMES];
    bool VolumeExists[SM_DRIVE_MAX_VOLUMES];
    uint8_t AtaIndex;
};

bool SMDrive_Constructor(struct SMDrive* self, struct Kernel* memory, uint8_t ide_device_id);
void SMDrive_Destructor(struct SMDrive* self);
struct MBRPartitionEntry* SMDrive_GetMBRPartitionEntry(struct MBR* mbr, uint8_t partition);

void SMDrive_ListVolumes(struct SMDrive* self);
struct SMVolume* SMDrive_GetVolume(struct SMDrive* self, uint8_t volume_id); 