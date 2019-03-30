#pragma once

#include <stdint.h>
#include <Drivers/ATA/ATA.h>
#include <Filesystem/FAT/BiosParameterBlock.h>
#include <Filesystem/FAT/Directory.h>
#include <Filesystem/MBR/MBR.h>

typedef struct
{
	uint8_t      AtaDeviceId;
	uint8_t      PartitionId;
	uint32_t     FirstSector;
	uint32_t     SectorCount;
	// MBR
	uint8_t       MasterBootRecordData[512];
	MBR_Record*   MasterBootRecord;
	// Bios Param Block
	uint8_t BiosParameterBlockData[512];
	FatBiosParameterBlock*   BiosParameterBlock;
	FatExtendedBootRecord16* ExtendedBootRecord16;
	FatExtendedBootRecord32* ExtendedBootRecord32;
	// Root Directory
	uint8_t RootDirSectorData[512];
	FatDirectory* RootDirectory;
	uint32_t RootDirSectorNumber;
} 
__attribute__((packed))
FatVolume;

FatVolume* FatVolume_Constructor(uint8_t ata_device_id, uint8_t partition_id);
void FatVolume_Destructor(FatVolume* partition);

uint32_t FatVolume_GetFirstSectorOfCluster
(uint32_t cluster, uint32_t sectors_per_cluster, uint32_t first_data_sector);

bool FatVolume_ReadMBR(FatVolume* volume);
bool FatVolume_ReadBPB(FatVolume* volume);
bool FatVolume_ReadRootDirectorySector(FatVolume* volume);

void FatVolume_DebugSector(uint8_t* sector);