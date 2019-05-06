#pragma once

#include <stdbool.h>
#include <Objects/FAT16/FatVolume.h>

#define VOLUME_NAME_SIZE 32

struct FatVolume;
struct SMDrive;
struct SMPath;
struct SMFile;
struct SMDirectoryEntry;

struct SMVolume
{
    bool Debug;
    bool Exists;
    struct SMDrive* ParentDrive;
    struct FatVolume FatVolume;
    uint8_t VolumeIndex;
    uint32_t FirstSectorIndex;
    uint32_t SectorsInPartition;
};

bool SMVolume_ATAConstructor(struct SMVolume* self, struct SMDrive* parent,  uint8_t partition_id, uint32_t first_sector, uint32_t sectors_in_partition);
bool SMVolume_FloppyConstructor(struct SMVolume* self, struct SMDrive* parent, uint32_t sectors_in_partition);
bool SMVolume_RamDiskConstructor(struct SMVolume* self, struct SMDrive* parent, uint32_t sectors_in_partition);
void SMVolume_Destructor(struct SMVolume* self);

bool SMVolume_GetDirectory(struct SMVolume* self, struct SMDirectoryEntry* dir, uint8_t* sector_buffer, uint32_t sector_count, struct SMPath* path);
void SMVolume_DebugRootDirectorySize(struct SMVolume* self);
uint16_t SMVolume_GetRootDirectoryCount(struct SMVolume* self);
uint32_t SMVolume_GetRootDirectorySectorCount(struct SMVolume* self);