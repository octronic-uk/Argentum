#pragma once

#include <stdint.h>

struct SMFileOffset
{
    uint32_t ByteOffset;
    uint32_t SectorsPerCluster; 
    uint32_t BytesPerSector;
    uint32_t Cluster;
    uint32_t SectorOffsetInCluster;
    uint32_t ByteOffsetIntoSector;
};
typedef struct SMFileOffset SMFileOffset;

void SMFileOffset_Debug(SMFileOffset* self);