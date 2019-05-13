#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "SMDrive.h"
#include "SMPath.h"
#include <Objects/LinkedList/LinkedList.h>

#define SM_MAX_FLOPPY_DRIVES 2
#define SM_MAX_ATA_DRIVES    4
#define SM_MAX_RAM_DISKS     4

typedef struct FILE FILE;

struct StorageManager
{
    bool Debug;
    // Ram Disk Objects
    LinkedList RamDisks; 
    // SM Drive Descriptors
    LinkedList Drives; 
    // Currently Open Files
    LinkedList OpenFiles;
};
typedef struct StorageManager StorageManager;

bool StorageManager_Constructor(StorageManager* self);
void StorageManager_Destructor(StorageManager *self);

void StorageManager_ListDrives(StorageManager* self);

SMDrive* StorageManager_GetATADrive(StorageManager* self, uint8_t drive_id);
SMDrive* StorageManager_GetFloppyDrive(StorageManager* self, uint8_t drive_id);
SMDrive* StorageManager_GetRamDiskDrive(StorageManager* self, uint8_t drive_id);

bool StorageManager_ProbeFloppyDrives(StorageManager* self);
bool StorageManager_ProbeATADrives(StorageManager* self);
void StorageManager_SetupRamDisk0(StorageManager* self);

bool StorageManager_Open(StorageManager* self, SMDirectoryEntry* dir, const char* path);
bool StorageManager_OpenPath(StorageManager* self, SMDirectoryEntry* dir, SMPath* path);
FILE* StorageManager_RequestFilePointer(StorageManager* self);
void StorageManager_CloseFilePointer(StorageManager* self, FILE* f);
