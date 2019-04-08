#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "SMDrive.h"
#include "SMPath.h"

#define SM_MAX_DRIVES 4

struct Kernel;

struct StorageManager
{
    bool Debug;
    struct Kernel* Kernel;
    struct SMDrive Drives[SM_MAX_DRIVES];
    bool DriveExists[SM_MAX_DRIVES];
};

bool StorageManager_Constructor(struct StorageManager* self, struct Kernel* kernel);
void StorageManager_Destructor(struct StorageManager* self);

void StorageManager_ListDrives(struct StorageManager* self);
struct SMDrive* StorageManager_GetDrive(struct StorageManager* self, uint8_t drive_id);

bool StorageManager_Open(struct StorageManager* self, struct SMDirectory* dir, const char* path);
bool StorageManager_OpenPath(struct StorageManager* self, struct SMDirectory* dir, struct SMPath* path);
