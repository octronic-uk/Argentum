#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MBR_BOOTSTRAP_CODE_1_SIZE     218
#define MBR_BOOTSTRAP_CODE_2_SIZE_216 216
#define MBR_BOOTSTRAP_CODE_2_SIZE_222 222

struct MbrChsAddress3b
{
    uint8_t CHS0;
    uint8_t CHS1;
    uint8_t CHS2;
}__attribute__((packed));

struct MbrChsAddress
{
    uint16_t Cylinder;
    uint8_t Head;
    uint8_t Sector;
}__attribute__((packed));

struct MbrPartitionEntry
{
    uint8_t Status;
    struct MbrChsAddress3b CHSFirstSector;
    uint8_t PartitionType;
    struct MbrChsAddress3b CHSLastSector;
    uint32_t LBAFirstSector;
    uint32_t SectorCount;
} __attribute__((packed));

struct ATA;
struct ATA_IDEDevice;



struct MbrRecord
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
    struct MbrPartitionEntry PartitionEntry1;
    struct MbrPartitionEntry PartitionEntry2;
    struct MbrPartitionEntry PartitionEntry3;
    struct MbrPartitionEntry PartitionEntry4;
    uint16_t BootSignature;
}__attribute__((packed)) ;

struct MBR
{
    bool Debug;
    struct ATA* ATA;
    struct ATA_IDEDevice* IDEDevice;
    struct MbrRecord Record;
};

bool MbrConstructor(struct MBR* self, struct ATA* ata, uint8_t ata_device_id);
void MbrChsAddress_From3b(struct MbrChsAddress* self, struct MbrChsAddress3b* data);

// Debug Functions 
void MbrRecord_Debug(struct MbrRecord* record);
void MbrPartitionEntry_Debug(int index, struct MbrPartitionEntry* pe);
void MbrChsAddress_Debug(const char* name, struct MbrChsAddress addr);