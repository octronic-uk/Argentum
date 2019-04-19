#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "FAT16/FatTableListing.h"

struct SMVolume;

struct SMDirectoryEntry
{
    struct SMVolume* Volume;
    struct FatTableListing FatListing;
    uint8_t FatAttributes;
};

bool SMDirectoryEntry_Constructor(struct SMDirectoryEntry* self, struct SMVolume* volume, uint32_t cluster, uint8_t fat_attributes);
void SMDirectoryEntry_Destructor(struct SMDirectoryEntry* self);
bool SMDirectoryEntry_IsFile(struct SMDirectoryEntry* self);