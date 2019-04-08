#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "FAT/FatDirectoryListing.h"

struct SMVolume;

struct SMDirectory
{
    struct SMVolume* Volume;
    struct FatDirectoryListing FatListing;
};

bool SMDirectory_Constructor(struct SMDirectory* self, struct SMVolume* volume, uint32_t cluster);
void SMDirectory_Destructor(struct SMDirectory* self);