#pragma once

#include <stdint.h>

struct SMFileOffset
{
    uint32_t SectorsPerCluster; 
    uint32_t SectorSize;
    uint32_t OffsetInSectors;
    uint32_t Cluster;
    uint32_t SectorInCluster;
    uint32_t ByteOffsetIntoSector;
};

void SMFileOffset_Debug(struct SMFileOffset* self);