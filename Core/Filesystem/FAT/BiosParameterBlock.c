#include <Filesystem/FAT/BiosParameterBlock.h>

#include <stdio.h>
#include <string.h>

void FatBPB_Debug(FatBiosParameterBlock* block)
{
    printf("FAT BPB:\n");
    printf("\tJMP:                   0x%x 0x%x 0x%x\n", block->Jmp[0], block->Jmp[1], block->Jmp[2]);
	printf("\tOEM ID:                %s\n",   block->OEMIdentifier);
	printf("\tBytes per Sector:      0x%x\n", block->BytesPerSector);
	printf("\tSectors per Cluster:   0x%x\n", block->SectorsPerCluster);
	printf("\tReserved Sector Count: 0x%x\n", block->ReservedSectorCount);
	printf("\tTable Count:           0x%x\n", block->TableCount);
	printf("\tRoot Entry Count:      0x%x\n", block->RootEntryCount);
	printf("\tTotal Sectors 16:      0x%x\n", block->TotalSectors16);
	printf("\tMedia Type:            0x%x\n", block->MediaType);
	printf("\tTable Size 16:         0x%x\n", block->TableSize16);
	printf("\tSectors Per Track:     0x%x\n", block->SectorsPerTrack);
	printf("\tHead Size Count:       0x%x\n", block->HeadSideCount);
	printf("\tHidden Sector Count:   0x%x\n", block->HiddenSectorCount);
	printf("\tTotal Sectors 32:      0x%x\n", block->TotalSectors32);
}

void FatBPB_DebugEBR16(FatExtendedBootRecord16* ebr)
{
	char vol_label[12], sys_id[9];
	memset(vol_label,0,12);
	memset(sys_id,0,12);

	memcpy(vol_label,ebr->VolumeLabelString,11);
	memcpy(sys_id,ebr->SystemIdentifierString,8);

	printf("\tFAT16 EBR Begins ----------\n");
	printf("\tDrive Number:             0x%x\n", ebr->DriveNumber);
	printf("\tFlags:                    0x%x\n", ebr->Flags);
	printf("\tSignature:                0x%x\n", ebr->Signature);
	printf("\tSerial Number:            0x%x\n", ebr->VolumeSerialNumber);
	printf("\tLabel:                    %s\n",   vol_label);
	printf("\tSytem ID String:          %s\n",   sys_id);
	printf("\tBoot Partition Signature: 0x%x\n", ebr->BootPartitionSignature);
}

void FatBPB_DebugEBR32(FatExtendedBootRecord32* ebr)
{
	printf("\tFAT32 EBR Begins ----------\n");
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
}

uint32_t FatBPB_GetTotalSectors(FatBiosParameterBlock* bpb)
{
	if (bpb->TotalSectors16 == 0)
	{
		return bpb->TotalSectors32; 
	}
	else
	{
		return (uint32_t)bpb->TotalSectors16;
	}
}

uint32_t FatBPB_GetFatSizeInSectors(FatBiosParameterBlock* bpb)
{
	if (bpb->TableSize16 == 0)
	{
		FatExtendedBootRecord32* bpb32;
		bpb32 = (FatExtendedBootRecord32*)&bpb->ExtendedBootRecord[0];
		return bpb32->SectorsPerFAT;
	}
	else
	{
		return bpb->TableSize16;
	}
}

uint32_t FatBPB_GetRootDirectorySizeInSectors(FatBiosParameterBlock* bpb)
{
	// 32 is the size of a FAT directory in bytes.
	return 
	(
		(bpb->RootEntryCount * 32) + 
		(bpb->BytesPerSector - 1)
	) / bpb->BytesPerSector;
}

uint32_t FatBPB_GetFirstDataSector(FatBiosParameterBlock* bpb)
{
	uint32_t first_data_sector = 
		bpb->ReservedSectorCount + 
		(bpb->TableCount * FatBPB_GetFatSizeInSectors(bpb)) + // was fat_size) + 
		FatBPB_GetRootDirectorySizeInSectors(bpb);//root_dir_sectors;
	return first_data_sector;
}

uint32_t FatBPB_GetFirstFATSector(FatBiosParameterBlock* bpb)
{
	return bpb->ReservedSectorCount;
}

uint32_t FatBPB_GetTotalDataSectors(FatBiosParameterBlock* bpb)
{
	uint32_t data_sectors = 
		FatBPB_GetTotalSectors(bpb) - 
		(
			bpb->ReservedSectorCount + 
			(bpb->TableCount * FatBPB_GetFatSizeInSectors(bpb)) + 
			FatBPB_GetRootDirectorySizeInSectors(bpb)
			//root_dir_sectors
		);
	return data_sectors;
}

uint32_t FatBPB_GetTotalClusters(FatBiosParameterBlock* bpb)
{
	return FatBPB_GetTotalDataSectors(bpb) / bpb->SectorsPerCluster;
}

FatType FatBPB_GetFATType(FatBiosParameterBlock* bpb)
{
	FatType fat_type;

	uint32_t total_clusters = FatBPB_GetTotalClusters(bpb);

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

uint32_t FatBPB_GetFirstRootDirectorySector(FatBiosParameterBlock* bpb)
{
	uint32_t first_data_sector = FatBPB_GetFirstDataSector(bpb);
	uint32_t root_dir_sectors = FatBPB_GetRootDirectorySizeInSectors(bpb);
	return first_data_sector - root_dir_sectors;
}
