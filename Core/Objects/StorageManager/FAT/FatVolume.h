#pragma once

#include <Drivers/ATA/ATADriver.h>
#include "FatConstants.h"
#include "FatBiosParameterBlock.h"
#include "FatDirectoryCluster.h"
#include "../MBR/MBR.h"

#define FAT_VOLUME_CLUSTER_UNUSED 0xE5
#define FAT_LFN_NAME_SIZE 128

struct Kernel;

struct FatVolume
{
	struct   Kernel*  Kernel;
	uint8_t  Debug;
	uint8_t  AtaDeviceId;
	uint8_t  PartitionId;
	uint32_t FirstSectorNumber;
	uint32_t FirstClusterNumber;
	uint32_t SectorCount;
	struct   FatBiosParameterBlock BiosParameterBlock;
	uint32_t RootDirSectorNumber;

} __attribute__((packed));

bool FatVolume_Constructor(struct FatVolume* self, struct Kernel* kernel, uint8_t ata_device_id, uint8_t partition_id, uint32_t lba_first_sector, uint32_t sector_count);
void FatVolume_Destructor(struct FatVolume* partition);
uint32_t FatVolume_GetFirstSectorOfCluster(struct FatVolume* self, uint32_t cluster);
bool FatVolume_ReadBPB(struct FatVolume* volume);
bool FatVolume_ReadSector(struct FatVolume* self, uint32_t sector,uint8_t* buffer);

void FatVolume_DebugSector(uint8_t* sector);