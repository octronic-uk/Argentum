#pragma once

#include <stdbool.h>

#include "FAT16/FatVolume.h"

#define VOLUME_NAME_SIZE 32

struct Kernel;
struct FatVolume;
struct Drive;
struct SMPath;
struct SMFile;
struct SMDirectoryEntry;

struct SMVolume
{
    bool Debug;
    struct SMDrive* ParentDrive;
    struct Kernel* Kernel;
    struct FatVolume FatVolume;
    uint8_t VolumeIndex;
    uint32_t FirstSectorIndex;
    uint32_t SectorsInPartition;
};

bool SMVolume_Constructor(
    struct SMVolume* self, 
    struct SMDrive* parent, 
    struct Kernel* kernel, 
    uint8_t partition_id, 
    uint32_t first_sector, 
    uint32_t sectors_in_partition
);

bool SMVolume_GetDirectory(struct SMVolume* self, struct SMDirectoryEntry* dir, uint8_t* sector_buffer, struct SMPath* path);