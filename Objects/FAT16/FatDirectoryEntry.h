#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
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

bool FatDirectoryEntry_Constructor
(struct FatDirectoryEntry* self, struct FatVolume* volume, struct FatDirectoryCluster* cluster, 
const char* name, uint32_t first_sector, uint32_t first_cluster);

void FatDirectoryEntry_Debug(struct FatDirectoryEntry* self);