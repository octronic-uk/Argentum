#include "FatBiosParameterBlock.h"

#include <stdio.h>
#include <string.h>

#include <Drivers/PS2/PS2Driver.h>

void FatBPB_Debug(struct FatBiosParameterBlock* self)
{
    printf("FAT BPB:\n");
    printf("\tJMP:                   0x%x 0x%x 0x%x\n", self->Jmp[0], self->Jmp[1], self->Jmp[2]);
	printf("\tOEM ID:                %s\n",   self->OEMIdentifier);
	printf("\tBytes per Sector:      0x%x\n", self->BytesPerSector);
	printf("\tSectors per Cluster:   0x%x\n", self->SectorsPerCluster);
	printf("\tReserved Sector Count: 0x%x\n", self->ReservedSectorCount);
	printf("\tTable Count:           0x%x\n", self->TableCount);
	printf("\tRoot Entry Count:      0x%x\n", self->RootEntryCount);
	printf("\tTotal Sectors 16:      0x%x\n", self->TotalSectors16);
	printf("\tMedia Type:            0x%x\n", self->MediaType);
	printf("\tTable Size 16:         0x%x\n", self->TableSize16);
	printf("\tSectors Per Track:     0x%x\n", self->SectorsPerTrack);
	printf("\tHead Size Count:       0x%x\n", self->HeadSideCount);
	printf("\tHidden Sector Count:   0x%x\n", self->HiddenSectorCount);
	printf("\tTotal Sectors 32:      0x%x\n", self->TotalSectors32);
    PS2Driver_WaitForKeyPress("FAT BPB Pause");
}

void FatBPB_DebugEBR16(struct FatExtendedBootRecord16* ebr)
{
	char vol_label[12], sys_id[9];
	memset(vol_label,0,12);
	memset(sys_id,0,12);

	memcpy(vol_label,ebr->VolumeLabelString,11);
	memcpy(sys_id,ebr->SystemIdentifierString,8);

	printf("FAT16 EBR:\n");
	printf("\tDrive Number:             0x%x\n", ebr->DriveNumber);
	printf("\tFlags:                    0x%x\n", ebr->Flags);
	printf("\tSignature:                0x%x\n", ebr->Signature);
	printf("\tSerial Number:            0x%x\n", ebr->VolumeSerialNumber);
	printf("\tLabel:                    %s\n",   vol_label);
	printf("\tSytem ID String:          %s\n",   sys_id);
	printf("\tBoot Partition Signature: 0x%x\n", ebr->BootPartitionSignature);
    PS2Driver_WaitForKeyPress("FAT BPB Pause");
}

void FatBPB_DebugEBR32(struct FatExtendedBootRecord32* ebr)
{
	printf("FAT32 EBR:\n");
	printf("\tSectors Per FAT:          0x%x\n",ebr->SectorsPerFAT);
	printf("\tFlags:                    0x%x\n",ebr->Flags);
	printf("\tFAT Version:              0x%x\n",ebr->FATVersionNumber);
	printf("\tRoot Directory Cluster:   0x%x\n",ebr->RootDirectoryClusterNumber);
	printf("\tFS Info Sector:           0x%x\n",ebr->FSInfoSectorNumber);
	printf("\tBackup Boot Sector:       0x%x\n",ebr->BackupBootSectorNumber);
	printf("\tDrive Number:             0x%x\n",ebr->DriveNumber);
	printf("\tNT Flags:                 0x%x\n",ebr->NTFlags);
	printf("\tSignature:                0x%x\n",ebr->Signature);
	printf("\tSerial Number:            0x%x\n",ebr->SerialNumber);
	printf("\tVolume Label:             %s\n",ebr->VolumeLabel);
	printf("\tSystem Identifier:        %s\n",ebr->SystemIdentifier);
	printf("\tBoot Pertition Signature: 0x%x\n",ebr->BootPartitionSignature);
    PS2Driver_WaitForKeyPress("FAT BPB Pause");
}

uint32_t FatBPB_GetTotalSectors(struct FatBiosParameterBlock* self)
{
	if (self->TotalSectors16 == 0)
	{
		return self->TotalSectors32; 
	}
	else
	{
		return (uint32_t)self->TotalSectors16;
	}
}

uint32_t FatBPB_GetFatSizeInSectors(struct FatBiosParameterBlock* self)
{
	if (self->TableSize16 == 0)
	{
		struct FatExtendedBootRecord32* ebr32;
		ebr32 = (struct FatExtendedBootRecord32*)&self->ExtendedBootRecord[0];
		return ebr32->SectorsPerFAT;
	}
	else
	{
		return self->TableSize16;
	}
}

uint32_t FatBPB_GetRootDirectorySizeInSectors(struct FatBiosParameterBlock* self)
{
	// 32 is the size of a FAT directory in bytes.
	return 
	(
		(self->RootEntryCount * 32) + 
		(self->BytesPerSector - 1)
	) / self->BytesPerSector;
}

uint32_t FatBPB_GetFirstDataSector(struct FatBiosParameterBlock* self)
{
	uint32_t first_data_sector = 
		self->ReservedSectorCount + 
		(self->TableCount * FatBPB_GetFatSizeInSectors(self)) + // was fat_size) + 
		FatBPB_GetRootDirectorySizeInSectors(self);//root_dir_sectors;
	return first_data_sector;
}

uint32_t FatBPB_GetFirstFATSector(struct FatBiosParameterBlock* self)
{
	return self->ReservedSectorCount;
}

uint32_t FatBPB_GetTotalDataSectors(struct FatBiosParameterBlock* self)
{
	uint32_t data_sectors = 
		FatBPB_GetTotalSectors(self) - 
		(
			self->ReservedSectorCount + 
			(self->TableCount * FatBPB_GetFatSizeInSectors(self)) + 
			FatBPB_GetRootDirectorySizeInSectors(self)
			//root_dir_sectors
		);
	return data_sectors;
}

uint32_t FatBPB_GetTotalClusters(struct FatBiosParameterBlock* self)
{
	return FatBPB_GetTotalDataSectors(self) / self->SectorsPerCluster;
}

enum FatType FatBPB_GetFATType(struct FatBiosParameterBlock* self)
{
	enum FatType fat_type;

	uint32_t total_clusters = FatBPB_GetTotalClusters(self);

	if(total_clusters < 4085) 
	{
		fat_type = FAT_12;
	} 
	else if(total_clusters < 65525) 
	{
		fat_type = FAT_16;
	} 
	else if (total_clusters < 268435445)
	{
		fat_type = FAT_32;
	}
	else
	{ 
		fat_type = FAT_EX;
	}
	return fat_type;
}

uint32_t FatBPB_GetFirstRootDirectorySector(struct FatBiosParameterBlock* self)
{
	uint32_t first_data_sector = FatBPB_GetFirstDataSector(self);
	uint32_t root_dir_sectors = FatBPB_GetRootDirectorySizeInSectors(self);
	return first_data_sector - root_dir_sectors;
}
