#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Objects/FAT/FatType.h>
#include <Objects/FAT/FatDirectoryListing.h>
#include <Objects/LinkedList/LinkedList.h>
#include <Objects/StorageManager/SMFileOffset.h>

struct SMVolume;

struct SMDirectoryEntry
{
    bool   Debug;
    struct SMVolume* Volume;
    struct FatDirectoryListing FatListing;
    struct LinkedList ClusterList;
    uint8_t FatAttributes;
    struct SMFileOffset FileOffset;
    uint16_t FirstCluster;
    uint32_t FirstSector;
};

bool SMDirectoryEntry_Constructor(struct SMDirectoryEntry* self, struct SMVolume* volume, uint16_t cluster, uint8_t fat_attributes);
void SMDirectoryEntry_Destructor(struct SMDirectoryEntry* self);
bool SMDirectoryEntry_IsFile(struct SMDirectoryEntry* self);
void SMDirectoryEntry_PopulateClusterList(struct SMDirectoryEntry* self, enum FatType type, uint16_t first_cluster);
void SMDirectoryEntry_PopulateClusterListFat12(struct SMDirectoryEntry* sself, uint16_t first_cluster);
void SMDirectoryEntry_PopulateClusterListFat16(struct SMDirectoryEntry* sself, uint16_t first_cluster);
void SMDirectoryEntry_SetFileOffset(struct SMDirectoryEntry* self, uint32_t offset);
bool SMDirectoryEntry_Read(struct SMDirectoryEntry* self, uint32_t offset, uint8_t* buffer);
bool SMDirectoryEntry_Write(struct SMDirectoryEntry* self, uint32_t offset, uint8_t* buffer);