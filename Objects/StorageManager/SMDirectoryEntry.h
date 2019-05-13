#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "SMFileOffset.h"
#include "FAT/FatType.h"
#include "FAT/FatDirectoryListing.h"
#include "FAT/FatDirectoryEntrySummary.h"
#include <Objects/LinkedList/LinkedList.h>

typedef struct SMVolume SMVolume;

struct SMDirectoryEntry
{
    bool   Debug;
    SMVolume* Volume;
    FatDirectoryListing FatListing;
    FatDirectoryEntrySummary FatDirectoryEntrySummary;
    LinkedList ClusterList;
    SMFileOffset FileOffset;
    uint32_t FirstSector;
};

typedef struct SMDirectoryEntry SMDirectoryEntry;


bool SMDirectoryEntry_Constructor(SMDirectoryEntry* self, SMVolume* volume, FatDirectoryEntrySummary* entry);
void SMDirectoryEntry_Destructor(SMDirectoryEntry* self);
bool SMDirectoryEntry_IsFile(SMDirectoryEntry* self);
void SMDirectoryEntry_PopulateClusterList(SMDirectoryEntry* self, enum FatType type, uint16_t first_cluster);
void SMDirectoryEntry_PopulateClusterListFat12(SMDirectoryEntry* sself, uint16_t first_cluster);
void SMDirectoryEntry_PopulateClusterListFat16(SMDirectoryEntry* sself, uint16_t first_cluster);
uint32_t SMDirectoryEntry_SetFileOffset(SMDirectoryEntry* self, uint32_t offset);
uint32_t SMDirectoryEntry_Read(SMDirectoryEntry* self, uint32_t offset, uint8_t* buffer);
uint32_t SMDirectoryEntry_Write(SMDirectoryEntry* self, uint32_t offset, uint8_t* buffer);