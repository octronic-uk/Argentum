#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "SMDrive.h"
#include "SMPath.h"
#include <Objects/LinkedList/LinkedList.h>

#define SM_MAX_FLOPPY_DRIVES 2
#define SM_MAX_ATA_DRIVES    4
#define SM_MAX_RAM_DISKS     4

struct StorageManager
{
    bool Debug;
    // Ram Disk Objects
    struct LinkedList RamDisks; 
    // SM Drive Descriptors
    struct SMDrive RamDiskDrives[SM_MAX_RAM_DISKS]; 
    struct SMDrive AtaDrives[SM_MAX_ATA_DRIVES];
    struct SMDrive FloppyDrives[SM_MAX_FLOPPY_DRIVES];
};

bool StorageManager_Constructor(struct StorageManager* self);
void StorageManager_Destructor(struct StorageManager *self);

void StorageManager_ListDrives(struct StorageManager* self);

struct SMDrive* StorageManager_GetATADrive(struct StorageManager* self, uint8_t drive_id);
struct SMDrive* StorageManager_GetFloppyDrive(struct StorageManager* self, uint8_t drive_id);
struct SMDrive* StorageManager_GetRamDiskDrive(struct StorageManager* self, uint8_t drive_id);

bool StorageManager_ProbeFloppyDrives(struct StorageManager* self);
bool StorageManager_ProbeATADrives(struct StorageManager* self);
void StorageManager_SetupRamDisk0(struct StorageManager* self);

bool StorageManager_Open(struct StorageManager* self, struct SMDirectoryEntry* dir, const char* path);
bool StorageManager_OpenPath(struct StorageManager* self, struct SMDirectoryEntry* dir, struct SMPath* path);

bool StorageManager_Test(struct StorageManager* self);
