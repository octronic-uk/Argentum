#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "FatDirectoryEntryData.h"
#include "FatConstants.h"

typedef struct FatVolume FatVolume;

struct FatDirectoryEntrySummary
{
    bool Debug;
    FatVolume* Volume;
    char Name[FAT_LFN_NAME_SIZE];
    uint32_t FileSize;
    uint16_t FirstCluster;
    uint32_t FirstSector;
    uint32_t PhysicalFirstSector;
    uint8_t  Attributes;
};

typedef struct FatDirectoryEntrySummary FatDirectoryEntrySummary;

bool FatDirectoryEntrySummary_Constructor
(
    FatDirectoryEntrySummary* self, FatVolume* volume, 
    const char* name, uint32_t first_sector, uint16_t first_cluster, 
    uint8_t attributes, uint32_t file_size
);

void FatDirectoryEntrySummary_Debug(FatDirectoryEntrySummary* self);