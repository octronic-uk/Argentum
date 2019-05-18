#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "SMFileOffset.h"
#include "FAT/FatType.h"
#include "FAT/FatDirectoryListing.h"
#include "FAT/FatDirectoryEntrySummary.h"
#include <Objects/Common/LinkedList/LinkedList.h>

typedef struct SMVolume SMVolume;

struct SMDirectoryEntry
{
    bool   Debug;
    SMVolume* Volume;
    FatDirectoryListing FatListing;
    FatDirectoryEntrySummary FatDirectoryEntrySummary;
    SMFileOffset FileOffset;
    enum FatType FatType;
    uint16_t FirstCluster;
};

typedef struct SMDirectoryEntry SMDirectoryEntry;


bool SMDirectoryEntry_Constructor(SMDirectoryEntry* self, SMVolume* volume, FatDirectoryEntrySummary* entry);
void SMDirectoryEntry_Destructor(SMDirectoryEntry* self);
bool SMDirectoryEntry_IsFile(SMDirectoryEntry* self);


uint32_t SMDirectoryEntry_SetFileOffset(SMDirectoryEntry* self, uint32_t offset);
uint32_t SMDirectoryEntry_Read(SMDirectoryEntry* self, uint32_t offset, uint8_t* buffer);
uint32_t SMDirectoryEntry_Write(SMDirectoryEntry* self, uint32_t offset, uint8_t* buffer);

uint16_t SMDirectoryEntry_GetNextCluster(SMDirectoryEntry* self, uint16_t cluster);
uint16_t SMDirectoryEntry_GetNextClusterFat12(SMDirectoryEntry* sself, uint16_t cluster);
uint16_t SMDirectoryEntry_GetNextClusterFat16(SMDirectoryEntry* sself, uint16_t cluster);
