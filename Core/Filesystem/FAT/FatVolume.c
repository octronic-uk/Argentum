#include <Filesystem/FAT/FatVolume.h>

#include <stdio.h>
#include <string.h>

#include <Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>
#include <Drivers/PS2/PS2Driver.h>

#include <Filesystem/MBR/MBR.h>
#include <Filesystem/FAT/FatDirectory.h>
#include <Filesystem/FAT/FatLongFileName.h>

extern struct Kernel kernel;

bool FatVolume_Constructor(struct FatVolume* self, struct ATADriver* ata, uint8_t ata_device_id, uint8_t partition_id)
{
    printf("FatVolume: Constructor\n");
    self->Debug = 1;
    self->ATA = ata;
    self->AtaDeviceId = ata_device_id;
    self->PartitionId = partition_id;

    if(!FatVolume_ReadMBR(self))
    {
        return false;
    }

    if (self->Debug)
    {
        printf("FatVolume: FirstSector: 0x%x, SectorCount: 0x%x\n", self->FirstSector, self->SectorCount);
    }

    if (!FatVolume_ReadBPB(self))
    {
        return false;
    }

    if(!FatVolume_ReadRootDirectorySector(self))
    {
        return false;
    }


    FatVolume_ParseEntriesInCluster(self,(uint8_t*)self->FirstCluster);
    return true;
}

bool FatVolume_ReadMBR(struct FatVolume* self)
{
    MBR_Constructor(&self->MasterBootRecord, self->ATA, self->AtaDeviceId);

    // Read the MBR to find out where the volume is located 
    switch (self->PartitionId)
    {
        case 0:
            self->FirstSector = self->MasterBootRecord.Record.PartitionEntry1.LBAFirstSector;
            self->SectorCount = self->MasterBootRecord.Record.PartitionEntry1.SectorCount;
            break;
        case 1:
            self->FirstSector = self->MasterBootRecord.Record.PartitionEntry2.LBAFirstSector;
            self->SectorCount = self->MasterBootRecord.Record.PartitionEntry2.SectorCount;
            break;
        case 2:
            self->FirstSector = self->MasterBootRecord.Record.PartitionEntry3.LBAFirstSector;
            self->SectorCount = self->MasterBootRecord.Record.PartitionEntry3.SectorCount;
            break;
        case 3:
            self->FirstSector = self->MasterBootRecord.Record.PartitionEntry4.LBAFirstSector;
            self->SectorCount = self->MasterBootRecord.Record.PartitionEntry4.SectorCount;
            break;        
        default:
            self->FirstSector = 0;
            self->SectorCount = 0;
            return false;
            break;
    }
    return true;
}

bool FatVolume_ReadBPB(struct FatVolume* self)
{
    // Read the BPB from first sector of partition

	uint8_t biosParameterBlockData[512];
    if (self->Debug) printf("FatVolume: Reading BPB...\n");
    if (!FatVolume_ReadSector(self,0,&biosParameterBlockData[0]))
    {
        printf("FatVolume: ATA error reading BPB\n");
        return false;
    }

    memcpy(&self->BiosParameterBlock, biosParameterBlockData,sizeof(struct FatBiosParameterBlock));

    struct FatBiosParameterBlock* bpb_tmp = (struct FatBiosParameterBlock*)biosParameterBlockData;
    memcpy(&self->ExtendedBootRecord16, bpb_tmp->ExtendedBootRecord, sizeof(struct FatExtendedBootRecord16));

    if (self->Debug)
    {
        FatBPB_Debug(&self->BiosParameterBlock);
        PS2Driver_WaitForKeyPress(&kernel.PS2);

        FatBPB_DebugEBR16(&self->ExtendedBootRecord16);
        PS2Driver_WaitForKeyPress(&kernel.PS2);
    }

    self->RootDirSectorNumber = FatBPB_GetFirstRootDirectorySector(&self->BiosParameterBlock);

    printf("FatVolume: Root Directory Sector at 0x%x\n", self->RootDirSectorNumber);
    return true;
}

bool FatVolume_ReadRootDirectorySector(struct FatVolume* self)
{
    if (!FatVolume_ReadSector(self, self->RootDirSectorNumber, &self->RootDirSectorData[0]))
    {
        printf("FatVolume: ATA Error while reading first data sector\n");
        return false;
    }

    FatVolume_DebugSector(self, self->RootDirSectorData); 

    self->RootDirectory = (struct FatDirectory*)&self->RootDirSectorData[0];

    FatDirectory_Debug(self->RootDirectory);

    PS2Driver_WaitForKeyPress(&kernel.PS2);

    if (!FatDirectory_HasAttribute(self->RootDirectory,FAT_DIR_ATTR_VOLUME_ID))
    {
        printf("FatVolume: Error - Root Directory does not have VOLUME_ID attribute\n");
        return false;
    }

    char vol_id[9];
    memset(vol_id,0,9);
    memcpy(vol_id,self->RootDirectory->Name,8);
    printf("FatVolume: Found Volume ID %s\n", vol_id);

    self->FirstCluster = (uint32_t) &self->RootDirSectorData[sizeof(struct FatDirectory)];
    return true;
}

void FatVolume_Destructor(struct FatVolume* part)
{
    printf("FatVolume: Destructor\n");
}

uint32_t FatVolume_GetFirstSectorOfCluster(struct FatVolume* self, uint32_t cluster)
{
    return FatBPB_GetFirstDataSector(&self->BiosParameterBlock) + ((cluster - 2) * self->BiosParameterBlock.SectorsPerCluster);
}

void FatVolume_DebugSector(struct FatVolume* self, uint8_t* sector)
{
    int i;
    printf("----------------------------------------\n");
    for (i=0; i<128; i++)
    {
        if (i % 8 == 0)
        {
            printf("\n");
        }
        printf("%x ",sector[i]);
    }
    printf("\n----------------------------------------\n");
}

/*
After the correct cluster has been loaded into memory, the next step is to read and parse all 
of the entries in it. Each entry is 32 bytes long. For each 32 byte entry this is the flow of 
execution:

    1 If the first byte of the entry is equal to 0 then there are no more files/directories in this 
      directory. 
        FirstByte==0, finish. 
        FirstByte!=0, goto 2.
    2 If the first byte of the entry is equal to 0xE5 then the entry is unused. 
        FirstByte==0xE5, goto 8, 
        FirstByte!=0xE5, goto 3.
    3 Is this entry a long file name entry? If the 11'th byte of the entry equals 0x0F, then it is a
      long file name entry. Otherwise, it is not. 
        11thByte==0x0F, goto 4. 
        11thByte!=0x0F, goto 5.
    4 Read the portion of the long filename into a temporary buffer. 
        Goto 8.
    5 Parse the data for this entry using the FatDirectory table. It would be a good
      idea to save the data for later. Possibly in a virtual file system structure. 
        goto 6
    6 Is there a long file name in the temporary buffer? 
        Yes, goto 7. 
        No, goto 8
    7 Apply the long file name to the entry that you just read and clear the temporary buffer. 
        goto 8
    8 Increment pointers and/or counters and check the next entry. 
        goto number 1
*/
void FatVolume_ParseEntriesInCluster(struct FatVolume* self, uint8_t* cluster_data)
{
    char lfn_current[FAT_VOLUME_LFN_NAME_BUFFER_SIZE];
    char lfn_tmp[FAT_VOLUME_LFN_NAME_BUFFER_SIZE];
    bool lfn_pending = 0;

    memset(lfn_current, 0, FAT_VOLUME_LFN_NAME_BUFFER_SIZE);
    memset(lfn_tmp, 0, FAT_VOLUME_LFN_NAME_BUFFER_SIZE);

    while(*cluster_data)    
    {
        if (*cluster_data != FAT_VOLUME_CLUSTER_UNUSED)
        {
            struct FatLongFileName* lfn = (struct FatLongFileName*)cluster_data;
            if (FatLongFileName_IsLfn(lfn))
            {
                lfn_pending = true;
                FatLongFileName_GetFileName(lfn, lfn_tmp);
                strcat(lfn_tmp,lfn_current);
                memcpy(lfn_current,lfn_tmp,FAT_VOLUME_LFN_NAME_BUFFER_SIZE);
                memset(lfn_tmp,0,FAT_VOLUME_LFN_NAME_BUFFER_SIZE);
            }
            else
            {
                // An LFN is pending
                if (lfn_pending)
                {
                    struct FatDirectory* lfn_dir = (struct FatDirectory*)cluster_data;
                    uint32_t lfn_first_cluster = lfn_dir->FirstClusterNumber;
                    uint32_t lfn_first_sector = FatVolume_GetFirstSectorOfCluster(self, lfn_first_cluster);
                    printf("FatVolume: Found LFN    : %s\n\tFirst Cluster at sector 0x%x (Phys 0x%x)\n",
                        lfn_current,
                        lfn_first_sector,
                        self->FirstSector + lfn_first_sector
                    );
                    memset(lfn_tmp,     0, FAT_VOLUME_LFN_NAME_BUFFER_SIZE);
                    memset(lfn_current, 0, FAT_VOLUME_LFN_NAME_BUFFER_SIZE);
                    lfn_pending = false;
                }
                else
                {
                    struct FatDirectory* dir = (struct FatDirectory*)cluster_data;
                    char name[9] = {0};
                    char ext[4] = {0};
                    memcpy(name,dir->Name,8);
                    memcpy(ext,dir->Extension,3);
                    uint32_t first_sector = FatVolume_GetFirstSectorOfCluster(self, dir->FirstClusterNumber);
                    printf("FatVolume: Found Non-LFN: %s.%s\n\tFirst Cluster at sector 0x%x (Phys 0x%x)\n",
                        name,
                        ext,
                        first_sector,
                        self->FirstSector + first_sector
                    );
                }
            }
        }
        else
        {
            printf("FatVolue: Unused Cluster\n");
        }
        cluster_data += 32;
    }
    printf("FatVolume: Reached end of cluster chain \n");
}

bool FatVolume_ReadSector(struct FatVolume* self, uint32_t sector,uint8_t* buffer)
{
    // Read the BPB from first sector of partition
    uint32_t physical_sector = self->FirstSector+sector;
    if (self->Debug) printf("FatVolume: Reading volume sector 0x%x (Physical sector 0x%x)\n",sector,physical_sector);
    uint8_t ata_error = ATADriver_IDEAccess(
        self->ATA,
        0, // Direction (Read)
        self->AtaDeviceId, // Device
        physical_sector, // LBA
        1, // NumSectors
        (void*)buffer// Buffer
    );

    if (ata_error)
    {
        printf("FatVolume: Error - ATA Error reading sector.\n");
        return false;
    }

    return true;
}