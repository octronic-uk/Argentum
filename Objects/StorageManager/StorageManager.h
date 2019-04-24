#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "SM_FloppyDrive.h"
#include "SM_ATADrive.h"
#include "SMPath.h"

#define SM_MAX_FLOPPY_DRIVES 2 
#define SM_MAX_ATA_DRIVES 4

struct StorageManager
{
    bool Debug;
    struct SM_FloppyDrive FloppyDrives[SM_MAX_FLOPPY_DRIVES];
    struct SM_ATADrive ATADrives[SM_MAX_ATA_DRIVES];
};

bool StorageManager_Constructor(struct StorageManager* self);

void StorageManager_ListDrives(struct StorageManager* self);
struct SM_ATADrive* StorageManager_GetATADrive(struct StorageManager* self, uint8_t drive_id);
struct SM_FloppyDrive* StorageManager_GetFloppyDrive(struct StorageManager* self, uint8_t drive_id);

bool StorageManager_ProbeFloppyDrives(struct StorageManager* self);
bool StorageManager_ProbeATADrives(struct StorageManager* self);

bool StorageManager_Open(struct StorageManager* self, struct SMDirectoryEntry* dir, const char* path);
bool StorageManager_OpenPath(struct StorageManager* self, struct SMDirectoryEntry* dir, struct SMPath* path);

bool StorageManager_Test(struct StorageManager* self);
