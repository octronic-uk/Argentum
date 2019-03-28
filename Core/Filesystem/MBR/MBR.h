#pragma once

#include <stdint.h>

#define MBR_BOOTSTRAP_CODE_1_SIZE     218
#define MBR_BOOTSTRAP_CODE_2_SIZE_216 216
#define MBR_BOOTSTRAP_CODE_2_SIZE_222 222

typedef struct
{
    uint8_t  BootstrapCode1Area[MBR_BOOTSTRAP_CODE_1_SIZE];
    uint16_t Blank1;
    uint8_t  OriginalPhysicalDrive;
    uint8_t  TimestampSeconds;
    uint8_t  MinutesSeconds;
    uint8_t  HoursSeconds;
    uint8_t  BootatrapCodeArea2[MBR_BOOTSTRAP_CODE_2_SIZE_216];
    uint32_t DiskSignature;
    uint16_t CopyProtected;
    uint16_t PartitionEntry1;
    uint16_t PartitionEntry2;
    uint16_t PartitionEntry3;
    uint16_t PartitionEntry4;
    uint16_t BootSignature;
} MBR_Record;

typedef union
{
    struct
    {
        uint8_t head     : 8;
        uint8_t sector   : 6;
        uint16_t cylinder : 10;
    };
    struct
    {
        uint8_t CHSb0;
        uint8_t CHSb1;
        uint8_t CHSb2;
    };
} MBR_CHSAddress;

typedef struct
{
    uint8_t        Status;
    MBR_CHSAddress CHSFirstSector;
    uint8_t        PartitionType;
    MBR_CHSAddress CHSLastSector;
    uint32_t       LBAFirstSector;
    uint32_t       SectorCount;
} MBR_PartitionEntry;


void MBR_DebugRecord(MBR_Record* record);