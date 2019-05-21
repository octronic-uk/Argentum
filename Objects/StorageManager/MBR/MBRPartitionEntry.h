#pragma once

#include <stdint.h>
#include "MBRChsAddress.h"

struct MBRPartitionEntry
{
    uint8_t Status;
    MBRChsAddress3b CHSFirstSector;
    uint8_t PartitionType;
    MBRChsAddress3b CHSLastSector;
    uint32_t LBAFirstSector;
    uint32_t SectorsInPartition;
} __attribute__((packed));

typedef struct MBRPartitionEntry MBRPartitionEntry;

void MBRPartitionEntry_Debug(MBRPartitionEntry* self, int index);