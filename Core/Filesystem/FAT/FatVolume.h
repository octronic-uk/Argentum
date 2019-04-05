#pragma once

#include <Drivers/ATA/ATADriver.h>
#include <Filesystem/FAT/FatBiosParameterBlock.h>
#include <Filesystem/FAT/FatDirectory.h>
#include <Filesystem/MBR/MBR.h>

#define FAT_VOLUME_CLUSTER_UNUSED 0xE5
#define FAT_VOLUME_LFN_NAME_BUFFER_SIZE 1024

struct Directory;
struct ATADriver;

struct FatVolume
{
	uint8_t Debug;
	struct ATADriver*  ATA;
	uint8_t      AtaDeviceId;
	uint8_t      PartitionId;
	uint32_t     FirstSector;
	uint32_t 	 FirstCluster;
	uint32_t     SectorCount;
	// MBR
	struct MBR  MasterBootRecord;
	// Bios Param Block
	struct FatBiosParameterBlock   BiosParameterBlock;
	struct FatExtendedBootRecord16 ExtendedBootRecord16;
	struct FatExtendedBootRecord32 ExtendedBootRecord32;
	// Root Directory
	uint8_t RootDirSectorData[512];
	struct FatDirectory* RootDirectory;
	uint32_t RootDirSectorNumber;
} __attribute__((packed));

bool FatVolume_Constructor(struct FatVolume* self, struct ATADriver* ata, uint8_t ata_device_id, uint8_t partition_id);
void FatVolume_Destructor(struct FatVolume* partition);
uint32_t FatVolume_GetFirstSectorOfCluster(struct FatVolume* self, uint32_t cluster);
bool FatVolume_ReadMBR(struct FatVolume* volume);
bool FatVolume_ReadBPB(struct FatVolume* volume);
bool FatVolume_ReadRootDirectorySector(struct FatVolume* volume);
void FatVolume_DebugSector(struct FatVolume* self, uint8_t* sector);
bool FatVolume_ReadSector(struct FatVolume* self, uint32_t sector,uint8_t* buffer);
void FatVolume_ParseEntriesInCluster(struct FatVolume* self, uint8_t* cluster);