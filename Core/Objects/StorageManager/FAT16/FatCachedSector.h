#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "FatConstants.h"

struct FatVolume;

struct FatCachedSector
{
    struct FatVolume* ParentVolume;
    uint32_t SectorNumber;
    uint8_t Data[FAT_SECTOR_SIZE];
    uint32_t LastAccess;
    bool Invalid;
    bool InUse;
};

bool FatCachedSector_Constructor(struct FatCachedSector* self, struct FatVolume* parent, uint32_t sector_number);
void FatCachedSector_Debug(struct FatCachedSector* self);