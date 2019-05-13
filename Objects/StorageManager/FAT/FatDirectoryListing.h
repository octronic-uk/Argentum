#pragma once

#include <stdbool.h>

#include "FatDirectoryEntryData.h"
#include "FatConstants.h"
#include "FatDirectoryEntrySummary.h"
#include <Objects/LinkedList/LinkedList.h>

typedef struct FatVolume FatVolume;

struct FatDirectoryListing
{
    bool Debug;
    char Name[FAT_LFN_NAME_SIZE];
    FatVolume* Volume;
    uint32_t EntryCount;
    LinkedList Entries;
};
typedef struct FatDirectoryListing FatDirectoryListing;

/**
    @brief Parse the Directory.
*/
bool FatDirectoryListing_Constructor
(FatDirectoryListing* self, FatVolume* volume, uint8_t* cluster_data, uint32_t sector_count);

void FatDirectoryListing_Destructor(FatDirectoryListing* self);
void FatDirectoryListing_Debug(FatDirectoryListing* self);