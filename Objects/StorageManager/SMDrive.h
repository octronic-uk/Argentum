#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "SMVolume.h"
#include "../MBR/MBR.h"

#define SM_DRIVE_MAX_VOLUMES 4

struct ATA_IDEDevice;
struct MBRPartitionEntry;
struct SMDirectoryEntry;

struct SMDrive
{
    bool Debug;
    bool Exists;
    struct SMVolume Volumes[SM_DRIVE_MAX_VOLUMES];
    // ATA
	struct MBR MasterBootRecord;
    uint8_t AtaIndex;
    // Floppy
    uint8_t FloppyIndex;
};

bool SMDrive_ATAConstructor(struct SMDrive* self, uint8_t ata_device_id);
bool SMDrive_FloppyConstructor(struct SMDrive* self, uint8_t floppy_device_id);

struct MBRPartitionEntry* SMDrive_ATAGetMBRPartitionEntry(struct MBR* mbr, uint8_t partition);
void SMDrive_ATAListVolumes(struct SMDrive* self);
struct SMVolume* SMDrive_GetVolume(struct SMDrive* self, uint8_t volume_id); 