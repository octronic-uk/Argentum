#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Objects/FAT16/FatType.h>
#include <Objects/FAT16/FatDirectoryListing.h>
#include <Objects/LinkedList/LinkedList.h>

struct SMVolume;

struct SMDirectoryEntry
{
    struct SMVolume* Volume;
    struct FatDirectoryListing FatListing;
    struct LinkedList ClusterList;
    uint8_t FatAttributes;
};

bool SMDirectoryEntry_Constructor(struct SMDirectoryEntry* self, struct SMVolume* volume, uint16_t cluster, uint8_t fat_attributes);
void SMDirectoryEntry_Destructor(struct SMDirectoryEntry* self);
bool SMDirectoryEntry_IsFile(struct SMDirectoryEntry* self);
void SMDirectoryEntry_PopulateClusterList(struct SMDirectoryEntry* self, enum FatType type, uint16_t first_cluster);
void SMDirectoryEntry_PopulateClusterListFat12(struct SMDirectoryEntry* sself, uint16_t first_cluster);
void SMDirectoryEntry_PopulateClusterListFat16(struct SMDirectoryEntry* sself, uint16_t first_cluster);