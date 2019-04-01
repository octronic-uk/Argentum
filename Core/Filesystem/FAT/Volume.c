#include "Volume.h"

#include <stdio.h>
#include <string.h>
#include <Kernel.h>
#include <Memory/Memory.h>
#include <Filesystem/MBR/MBR.h>
#include <Drivers/PS2/PS2.h>
#include <Filesystem/FAT/Directory.h>
#include <Filesystem/FAT/LongFileName.h>
#include <Filesystem/API/Directory.h>

uint8_t FatVolume_Debug = 1;

extern struct Kernel kernel;

bool FatVolume_Constructor(struct FatVolume* self, struct ATA* ata, uint8_t ata_device_id, uint8_t partition_id)
{
    printf("FatVolume Constructor\n");
    self->ATA = ata;
    self->AtaDeviceId = ata_device_id;
    self->PartitionId = partition_id;

    if(!FatVolume_ReadMBR(self))
    {
        return false;
    }

    if (FatVolume_Debug)
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
}

bool FatVolume_GetRootDirectory(struct FatVolume* self, struct Directory* dir)
{
    uint8_t* first_cluster = self->RootDirSectorData+sizeof(struct FatDirectory);
    FatVolume_DebugSector(self, first_cluster);

    printf("FatVolume: Reading Root Directory\n");
    printf("FatVolume: Checking for LFN...\n");

    struct FatLongFileName* lfn = (struct FatLongFileName*)first_cluster;
    if (!FatLongFileName_IsLfn(lfn))
    {
        printf("FatVolume: This entry is not an LFN\n");
        struct FatDirectory* dir = (struct FatDirectory*)first_cluster;
    }
    else
    {
        FatLongFileName_Debug(lfn);
    }

    PS2_WaitForKeyPress(&kernel.PS2);
    return 0;
}

bool FatVolume_ReadMBR(struct FatVolume* self)
{
    MbrConstructor(&self->MasterBootRecord, self->ATA, self->AtaDeviceId);

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
    if (FatVolume_Debug) printf("FatVolume: Reading BPB...\n");
    if (!FatVolume_ReadSector(self,0,&biosParameterBlockData[0]))
    {
        printf("FatVolume: ATA error reading BPB\n");
        return false;
    }

    memcpy(&self->BiosParameterBlock, biosParameterBlockData,sizeof(struct FatBiosParameterBlock));

    struct FatBiosParameterBlock* bpb_tmp = (struct FatBiosParameterBlock*)biosParameterBlockData;
    memcpy(&self->ExtendedBootRecord16, bpb_tmp->ExtendedBootRecord, sizeof(struct FatExtendedBootRecord16));

    if (FatVolume_Debug)
    {
        FatBPB_Debug(&self->BiosParameterBlock);
        PS2_WaitForKeyPress(&kernel.PS2);

        FatBPB_DebugEBR16(&self->ExtendedBootRecord16);
        PS2_WaitForKeyPress(&kernel.PS2);
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

    PS2_WaitForKeyPress(&kernel.PS2);

    if (!FatDirectory_HasAttribute(self->RootDirectory,FAT_DIR_ATTR_VOLUME_ID))
    {
        printf("FatVolume: Error - Root Directory does not have VOLUME_ID attribute\n");
        return false;
    }

    char vol_id[9];
    memset(vol_id,0,9);
    memcpy(vol_id,self->RootDirectory->Name,8);
    printf("FatVolume: Found Volume ID %s\n", vol_id);

    return true;
}

void FatVolume_Destructor(struct FatVolume* part)
{
    printf("FatVolume Destructor\n");
    Memory_Free(&kernel.Memory, part);
}

uint32_t FatVolume_GetFirstSectorOfCluster(struct FatVolume* self, uint32_t cluster, uint32_t sectors_per_cluster, uint32_t first_data_sector)
{
    return first_data_sector + ((cluster - 2) * sectors_per_cluster);
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

    * If the first byte of the entry is equal to 0 then there are no more files/directories in this 
      directory. FirstByte==0, finish. FirstByte!=0, goto 2.
    * If the first byte of the entry is equal to 0xE5 then the entry is unused. FirstByte==0xE5, 
      goto 8, FirstByte!=0xE5, goto 3.
    * Is this entry a long file name entry? If the 11'th byte of the entry equals 0x0F, then it is a
      long file name entry. Otherwise, it is not. 11thByte==0x0F, goto 4. 11thByte!=0x0F, goto 5.
    * Read the portion of the long filename into a temporary buffer. Goto 8.
    * Parse the data for this entry using the table from further up on this page. It would be a good
      idea to save the data for later. Possibly in a virtual file system structure. goto 6
    * Is there a long file name in the temporary buffer? Yes, goto 7. No, goto 8
    * Apply the long file name to the entry that you just read and clear the temporary buffer. goto
      8
    * Increment pointers and/or counters and check the next entry. (goto number 1)
*/
void FatVolume_ParseEntriesInCluster(struct FatVolume* self, uint8_t* cluster)
{
    
}

bool FatVolume_ReadSector(struct FatVolume* self, uint32_t sector,uint8_t* buffer)
{
    // Read the BPB from first sector of partition
    uint32_t physical_sector = self->FirstSector+sector;
    if (FatVolume_Debug) printf("FatVolume: Reading volume sector 0x%x (Physical sector 0x%x)\n",sector,physical_sector);
    uint8_t ata_error = ATA_IDEAccess(
        self->ATA,
        0, // Direction (Read)
        self->AtaDeviceId, // Device
        physical_sector, // LBA
        1, // NumSectors
        0, // Selector
        (uint32_t)buffer// Buffer
    );

    if (ata_error)
    {
        printf("FatVolume: Error - ATA Error reading sector.\n");
        return false;
    }

    return true;
}