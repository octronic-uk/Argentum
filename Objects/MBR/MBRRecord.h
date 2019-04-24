#pragma once

#include <stdint.h>
#include "MBRPartitionEntry.h"

#define MBR_BOOTSTRAP_CODE_1_SIZE     218
#define MBR_BOOTSTRAP_CODE_2_SIZE_216 216
#define MBR_BOOTSTRAP_CODE_2_SIZE_222 222

struct MBRRecord
{
    uint8_t  BootstrapCodeArea1[MBR_BOOTSTRAP_CODE_1_SIZE];
    uint16_t Blank1;
    uint8_t  OriginalPhysicalDrive;
    uint8_t  TimestampSeconds;
    uint8_t  TimestampMinutes;
    uint8_t  TimestampHours;
    uint8_t  BootstrapCodeArea2[MBR_BOOTSTRAP_CODE_2_SIZE_216];
    uint32_t DiskSignature;
    uint16_t CopyProtected;
    struct MBRPartitionEntry PartitionEntry1;
    struct MBRPartitionEntry PartitionEntry2;
    struct MBRPartitionEntry PartitionEntry3;
    struct MBRPartitionEntry PartitionEntry4;
    uint16_t BootSignature;
}__attribute__((packed));

void MBRRecord_Debug(struct MBRRecord* record);