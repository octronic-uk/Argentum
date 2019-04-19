#pragma once

#include <Drivers/ATA/ATADriver.h>
#include "FatConstants.h"
#include "FatBiosParameterBlock.h"
#include "FatDirectoryCluster.h"
#include "FatCachedSector.h"

struct Kernel;

/**
 @brief Contains data relating to a FAT volume including the Bios Parameter Block
 */
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
	struct   FatCachedSector SectorCache[FAT_CACHED_SECTORS_SIZE];
	uint32_t RootDirSectorNumber;
} __attribute__((packed));

/**
    @brief Construct a FatVolume object
 */
bool FatVolume_Constructor(struct FatVolume* self, struct Kernel* kernel, uint8_t ata_device_id, 
	uint8_t partition_id, uint32_t lba_first_sector, uint32_t sector_count);

/**
    @brief Destruct a FatVolume object
 */
void FatVolume_Destructor(struct FatVolume* partition);

/**
	@brief Get the first sector of the given cluster
*/
uint32_t FatVolume_GetFirstSectorOfCluster(struct FatVolume* self, uint32_t cluster);

/**
	@brief Read the Bios Parameter Block from the given FatVolume
*/
bool FatVolume_ReadBPB(struct FatVolume* volume);

/*
	@brief Read the given sector from the FatVolume into buffer. Sector is indexed from the start
	of the volume, not from the start of the physical disk.
*/
bool FatVolume_ReadSector(struct FatVolume* self, uint32_t sector,uint8_t* buffer);

/*
	@brief Write the given sector from buffer to the FatVolume. Sector is indexed from the start
	of the volume, not from the start of the physical disk.
*/
bool FatVolume_WriteSector(struct FatVolume* self, uint32_t sector_to_write, uint8_t* buffer);

/**
	@brief Print out the contents of the given sector for debugging
*/
void FatVolume_DebugSector(uint8_t* sector);

/**
	@brief Get the next sector cache to use
 */

struct FatCachedSector* FatVolume_GetNextAvailableCachedSector(struct FatVolume* volume);