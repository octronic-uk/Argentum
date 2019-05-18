#pragma once

#include "FatConstants.h"
#include "FatBiosParameterBlock.h"
#include "FatDirectoryEntryData.h"

/**
 @brief Contains data relating to a FAT volume including the Bios Parameter Block
 */
struct FatVolume
{
	uint8_t  Debug;
	int8_t   FloppyDeviceIndex;
	int8_t   AtaDeviceIndex;
	int8_t   PartitionIndex;
	int8_t   RamDiskIndex;
	uint32_t FirstSectorNumber;
	uint32_t FirstClusterNumber;
	uint32_t SectorCount;
	FatBiosParameterBlock BiosParameterBlock;
	uint8_t* FileAllocationTableData;
	uint32_t RootDirSectorNumber;
} __attribute__((packed));

typedef struct FatVolume FatVolume;
/**
    @brief Construct a FatVolume object for an ATA Drive
 */
bool FatVolume_ATAConstructor(FatVolume* self, uint8_t ata_device_index, uint8_t partition_index, uint32_t lba_first_sector, uint32_t sector_count);

/**
    @brief Construct a FatVolume object for a Floppy Drive
 */
bool FatVolume_FloppyConstructor(FatVolume* self, uint8_t device_index);

/**
    @brief Construct a FatVolume object for a RamDisk
 */
bool FatVolume_RamDiskConstructor(FatVolume* self, uint8_t device_index);

/**
 	@brief Destruct a FatVolume object
 */
void FatVolume_Destructor(FatVolume* self);

/**
	@brief Get the first sector of the given cluster
*/
uint32_t FatVolume_GetFirstSectorOfCluster(FatVolume* self, uint32_t cluster);

/**
	@brief Read the Bios Parameter Block from the given FatVolume
*/
bool FatVolume_ReadBPB(FatVolume* volume);

/*
	@brief Read the given sector from the FatVolume into buffer. Sector is indexed from the start
	of the volume, not from the start of the physical disk.
*/
bool FatVolume_ReadSector(FatVolume* self, uint32_t sector,uint8_t* buffer);

/*
	@brief Write the given sector from buffer to the FatVolume. Sector is indexed from the start
	of the volume, not from the start of the physical disk.
*/
bool FatVolume_WriteSector(FatVolume* self, uint32_t sector_to_write, uint8_t* buffer);

/**
	@brief Print out the contents of the given sector for debugging
*/
void FatVolume_DebugSector(uint8_t* sector);

enum FatType FatVolume_GetFatType(FatVolume* self);

bool FatVolume_ReadFileAllocationTableData(FatVolume* self);