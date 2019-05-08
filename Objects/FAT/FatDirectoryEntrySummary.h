#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "FatDirectoryEntryData.h"
#include "FatConstants.h"

struct FatVolume;

struct FatDirectoryEntrySummary
{
    bool Debug;
    struct FatVolume* Volume;
    char Name[FAT_LFN_NAME_SIZE];
    uint32_t FileSize;
    uint16_t FirstCluster;
    uint32_t FirstSector;
    uint32_t PhysicalFirstSector;
    uint8_t  Attributes;
};

bool FatDirectoryEntrySummary_Constructor
(
    struct FatDirectoryEntrySummary* self, struct FatVolume* volume, 
    const char* name, uint32_t first_sector, uint16_t first_cluster, 
    uint8_t attributes, uint32_t file_size
);

void FatDirectoryEntrySummary_Debug(struct FatDirectoryEntrySummary* self);