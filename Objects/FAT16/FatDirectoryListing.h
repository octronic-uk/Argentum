#pragma once

#include <stdbool.h>

#include "FatDirectoryEntryData.h"
#include "FatConstants.h"
#include "FatDirectoryEntrySummary.h"
#include <Objects/LinkedList/LinkedList.h>

struct FatVolume;

struct FatDirectoryListing
{
    bool Debug;
    char Name[FAT_LFN_NAME_SIZE];
    struct FatVolume* Volume;
    uint32_t EntryCount;
    struct LinkedList Entries;
};

/**
    @brief Parse the Directory.
*/
bool FatDirectoryListing_Constructor
(struct FatDirectoryListing* self, struct FatVolume* volume, uint8_t* cluster_data, uint32_t sector_count);

void FatDirectoryListing_Destructor(struct FatDirectoryListing* self);
void FatDirectoryListing_Debug(struct FatDirectoryListing* self);