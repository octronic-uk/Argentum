#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "SMVolume.h"
#include "MBR/MBR.h"

#define SM_DRIVE_MAX_VOLUMES 4

struct ATA_IDEDevice;
struct MBRPartitionEntry;
struct SMDirectoryEntry;

struct SM_FloppyDrive
{
    bool Debug;
    bool Exists;
    struct SMVolume Volume;
    uint8_t DriveIndex;
};

bool SM_FloppyDrive_Constructor(struct SM_FloppyDrive* self, uint8_t floppy_drive_id);