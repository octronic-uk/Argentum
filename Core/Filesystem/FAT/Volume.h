#pragma once

#include <stdint.h>
#include <Drivers/ATA/ATA.h>
#include <Filesystem/FAT/BiosParameterBlock.h>
#include <Filesystem/FAT/Directory.h>
#include <Filesystem/MBR/MBR.h>

struct Directory;

struct FatVolume
{
	struct ATA*  ATA;
	uint8_t      AtaDeviceId;
	uint8_t      PartitionId;
	uint32_t     FirstSector;
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

bool FatVolume_Constructor(struct FatVolume* self, struct ATA* ata, uint8_t ata_device_id, uint8_t partition_id);
void FatVolume_Destructor(struct FatVolume* partition);
uint32_t FatVolume_GetFirstSectorOfCluster(struct FatVolume* self, uint32_t cluster, uint32_t sectors_per_cluster, uint32_t first_data_sector);
bool FatVolume_ReadMBR(struct FatVolume* volume);
bool FatVolume_ReadBPB(struct FatVolume* volume);
bool FatVolume_ReadRootDirectorySector(struct FatVolume* volume);
void FatVolume_DebugSector(struct FatVolume* self, uint8_t* sector);
bool FatVolume_ReadSector(struct FatVolume* self, uint32_t sector,uint8_t* buffer);
bool FatVolume_GetRootDirectory(struct FatVolume* volume, struct Directory* directory);