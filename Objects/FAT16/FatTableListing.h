#pragma once

#include <stdbool.h>

#include "FatDirectoryEntryData.h"
#include "FatConstants.h"
#include "FatDirectoryEntrySummary.h"
#include <Objects/LinkedList/LinkedList.h>

struct FatVolume;

struct FatTableListing
{
    bool Debug;
    char Name[FAT_LFN_NAME_SIZE];
    struct FatVolume* Volume;
    uint32_t EntryCount;
    //struct FatDirectoryEntrySummary Entries[FAT_DIR_LISTING_MAX_DIRS];
    struct LinkedList Entries;
};

/**
    @brief Parse the FAT Table.
*/
bool FatTableListing_Constructor
(struct FatTableListing* self, struct FatVolume* volume, uint8_t* cluster_data, uint32_t sector_count);

void FatTableListing_Destructor(struct FatTableListing* self);
void FatTableListing_Debug(struct FatTableListing* self);