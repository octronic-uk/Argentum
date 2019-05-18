#pragma once

#include <stdbool.h>
#include "FAT/FatVolume.h"

#define VOLUME_NAME_SIZE 32

typedef struct FatVolume FatVolume;
typedef struct SMDrive SMDrive;
typedef struct SMPath SMPath;
typedef struct SMFile SMFile;
typedef struct SMDirectoryEntry SMDirectoryEntry;

struct SMVolume
{
    bool Debug;
    bool Exists;
    SMDrive* ParentDrive;
    FatVolume FatVolume;
    uint8_t VolumeIndex;
    uint32_t FirstSectorIndex;
    uint32_t SectorsInPartition;
};

typedef struct SMVolume SMVolume;

bool SMVolume_ATAConstructor(SMVolume* self, SMDrive* parent,  uint8_t partition_id, uint32_t first_sector, uint32_t sectors_in_partition);
bool SMVolume_FloppyConstructor(SMVolume* self, SMDrive* parent, uint32_t sectors_in_partition);
bool SMVolume_RamDiskConstructor(SMVolume* self, SMDrive* parent, uint32_t sectors_in_partition);
void SMVolume_Destructor(SMVolume* self);

bool SMVolume_GetDirectory(SMVolume* self, SMDirectoryEntry* dir, uint8_t* sector_buffer, uint32_t sector_count, SMPath* path);
void SMVolume_DebugRootDirectorySize(SMVolume* self);
uint16_t SMVolume_GetRootDirectoryCount(SMVolume* self);
uint32_t SMVolume_GetRootDirectorySectorCount(SMVolume* self);