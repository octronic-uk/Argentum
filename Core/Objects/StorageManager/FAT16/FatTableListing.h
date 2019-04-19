#pragma once

#include <stdbool.h>

#include "FatDirectoryCluster.h"
#include "FatConstants.h"
#include "FatDirectoryEntry.h"

struct FatVolume;

struct FatTableListing
{
    bool Debug;
    char Name[FAT_LFN_NAME_SIZE];
    struct FatVolume* Volume;
    uint32_t EntryCount;
    struct FatDirectoryEntry Entries[FAT_DIR_LISTING_MAX_DIRS];
};

bool FatTableListing_Constructor(struct FatTableListing* self, struct FatVolume* volume, uint8_t* cluster_data);
void FatTableListing_Debug(struct FatTableListing* self);