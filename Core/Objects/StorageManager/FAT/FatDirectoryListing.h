#pragma once

#include <stdbool.h>

#include "FatDirectoryCluster.h"
#include "FatConstants.h"


struct FatVolume;

struct FatDirectoryEntry
{
    bool Debug;
    struct FatVolume* Volume;
    struct FatDirectoryCluster ClusterData;
    char Name[FAT_LFN_NAME_SIZE];
    uint32_t FirstCluster;
    uint32_t FirstSector;
    uint32_t PhysicalFirstSector;
};

struct FatDirectoryListing
{
    bool Debug;
    char Name[FAT_LFN_NAME_SIZE];
    struct FatVolume* Volume;
    uint32_t EntryCount;
    struct FatDirectoryEntry Entries[FAT_DIR_LISTING_MAX_DIRS];
};

bool FatDirectoryListing_Constructor
(struct FatDirectoryListing* self, struct FatVolume* volume, uint8_t* cluster_data);

bool FatDirectoryEntry_Constructor
(struct FatDirectoryEntry* self, struct FatVolume* volume, struct FatDirectoryCluster* cluster, 
const char* name, uint32_t first_sector, uint32_t first_cluster);

void FatDirectoryListing_Debug(struct FatDirectoryListing* self);