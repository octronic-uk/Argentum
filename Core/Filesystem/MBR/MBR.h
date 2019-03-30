#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Drivers/ATA/ATA.h>

#define MBR_BOOTSTRAP_CODE_1_SIZE     218
#define MBR_BOOTSTRAP_CODE_2_SIZE_216 216
#define MBR_BOOTSTRAP_CODE_2_SIZE_222 222

typedef struct
{
    uint8_t CHS0;
    uint8_t CHS1;
    uint8_t CHS2;
}__attribute__((packed)) 
MBR_CHSAddress3b;

typedef struct
{
    uint16_t Cylinder;
    uint8_t Head;
    uint8_t Sector;
}__attribute__((packed)) 
MBR_CHSAddress;

typedef struct
{
    uint8_t Status;
    MBR_CHSAddress3b CHSFirstSector;
    uint8_t PartitionType;
    MBR_CHSAddress3b CHSLastSector;
    uint32_t LBAFirstSector;
    uint32_t SectorCount;
} __attribute__((packed)) 
MBR_PartitionEntry;

typedef struct
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
    MBR_PartitionEntry PartitionEntry1;
    MBR_PartitionEntry PartitionEntry2;
    MBR_PartitionEntry PartitionEntry3;
    MBR_PartitionEntry PartitionEntry4;
    uint16_t BootSignature;
}__attribute__((packed)) 
MBR_Record;

MBR_Record* MBR_RecordConstructor(uint8_t ata_device_id);
bool MBR_RecordPreallocatedConstructor(MBR_Record* record, uint8_t ata_device_id);
MBR_CHSAddress MBR_ConvertCHSAddress3b(MBR_CHSAddress3b* data);

// Debug Functions 
void MBR_DebugRecord(MBR_Record* record);
void MBR_DebugPartitionEntry(int index, MBR_PartitionEntry* pe);
void MBR_DebugCHSAddress(const char* name, MBR_CHSAddress addr);