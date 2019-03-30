#include "Volume.h"

#include <stdio.h>
#include <string.h>
#include <Memory/Memory.h>
#include <Filesystem/MBR/MBR.h>
#include <Drivers/PS2/Intel8042.h>
#include <Filesystem/FAT/Directory.h>

uint8_t FatVolume_Debug = 1;

FatVolume* FatVolume_Constructor(uint8_t ata_device_id, uint8_t partition_id)
{
    printf("FatVolume Constructor\n");
    FatVolume* volume = Memory_Allocate(sizeof (FatVolume));
    volume->AtaDeviceId = ata_device_id;
    volume->PartitionId = partition_id;

    if(!FatVolume_ReadMBR(volume))
    {
        Memory_Free(volume);
        return 0;
    }

    if (FatVolume_Debug)
    {
        printf("FatVolume: FirstSector: 0x%x, SectorCount: 0x%x\n",
            volume->FirstSector, volume->SectorCount);
    }
    I8042_WaitForKeyPress();

    if (!FatVolume_ReadBPB(volume))
    {
        Memory_Free(volume);
        return 0;
    }
    I8042_WaitForKeyPress();

    if(!FatVolume_ReadRootDirectorySector(volume))
    {
        Memory_Free(volume);
        return 0;
    }
    I8042_WaitForKeyPress();

    /*
    uint32_t first_cluster_number = root_directory->FirstClusterNumber;
    

    uint32_t first_cluster_sector = FatVolume_GetFirstSectorOfCluster(
        first_cluster_number, bpb->SectorsPerCluster, volume->FirstSector);

    printf("FatVolume: First Cluster Number 0x%x, Sector 0x%x\n",
            first_cluster_number, first_cluster_sector);

    uint8_t first_cluster_data[512];
    memset(first_cluster_data,0,512);

    ata_error = ATA_IDEAccess(
        0, // Direction (Read)
        volume->AtaDeviceId, // Device
        // Remember to use the volume's first sector as the base
        first_cluster_sector, // LBA
        1, // NumSectors
        0, // Selector
        (uint32_t)first_cluster_data // Buffer
    );

    FatVolume_DebugSector(first_cluster_data);
    */

    return volume;
}

bool FatVolume_ReadMBR(FatVolume* volume)
{
    volume->MasterBootRecord = (MBR_Record*)&volume->MasterBootRecordData[0]; 
    MBR_RecordPreallocatedConstructor(volume->MasterBootRecord, volume->AtaDeviceId);

    // Read the MBR to find out where the volume is located 
    switch (volume->PartitionId)
    {
        case 0:
            volume->FirstSector = volume->MasterBootRecord->PartitionEntry1.LBAFirstSector;
            volume->SectorCount = volume->MasterBootRecord->PartitionEntry1.SectorCount;
            break;
        case 1:
            volume->FirstSector = volume->MasterBootRecord->PartitionEntry2.LBAFirstSector;
            volume->SectorCount = volume->MasterBootRecord->PartitionEntry2.SectorCount;
            break;
        case 2:
            volume->FirstSector = volume->MasterBootRecord->PartitionEntry3.LBAFirstSector;
            volume->SectorCount = volume->MasterBootRecord->PartitionEntry3.SectorCount;
            break;
        case 3:
            volume->FirstSector = volume->MasterBootRecord->PartitionEntry4.LBAFirstSector;
            volume->SectorCount = volume->MasterBootRecord->PartitionEntry4.SectorCount;
            break;        
        default:
            volume->FirstSector = 0;
            volume->SectorCount = 0;
            return false;
            break;
    }
    return true;
}

bool FatVolume_ReadBPB(FatVolume* volume)
{
    // Read the BPB from first sector of partition
    if (FatVolume_Debug) printf("FatVolume: Reading BPB...\n");
    uint8_t ata_error = ATA_IDEAccess(
        0,                     // Direction (Read)
        volume->AtaDeviceId,   // Device
        volume->FirstSector,   // LBA
        1,                     // NumSectors
        0,                     // Selector
        (uint32_t)&volume->BiosParameterBlockData[0] // Buffer
    );

    if (ata_error)
    {
        printf("FatVolume: ATA error reading BPB\n");
        return false;
    }

    volume->BiosParameterBlock = (FatBiosParameterBlock*)&volume->BiosParameterBlockData[0];
    volume->ExtendedBootRecord16 = (FatExtendedBootRecord16*)volume->BiosParameterBlock->ExtendedBootRecord;

    if (FatVolume_Debug)
    {
        FatBPB_Debug(volume->BiosParameterBlock);
        I8042_WaitForKeyPress();

        FatBPB_DebugEBR16(volume->ExtendedBootRecord16);
        I8042_WaitForKeyPress();
    }

    volume->RootDirSectorNumber = FatBPB_GetFirstRootDirectorySector(volume->BiosParameterBlock);

    printf("FatVolume: Root Directory Sector at 0x%x\n", volume->RootDirSectorNumber);
    return true;
}

bool FatVolume_ReadRootDirectorySector(FatVolume* volume)
{
    uint8_t ata_error = ATA_IDEAccess(
        0, // Direction (Read)
        volume->AtaDeviceId, // Device
        // Remember to use the volume's first sector as the base
        volume->FirstSector + volume->RootDirSectorNumber, // LBA
        1, // NumSectors
        0, // Selector
        (uint32_t)&volume->RootDirSectorData[0] // Buffer
    );

    if (ata_error)
    {
        printf("FatVolume: ATA Error while reading first data sector\n");
        return false;
    }

    FatVolume_DebugSector(volume->RootDirSectorData); 

    volume->RootDirectory = (FatDirectory*)&volume->RootDirSectorData[0];

    FatDirectory_Debug(volume->RootDirectory);

    I8042_WaitForKeyPress();

    if (!FatDirectory_HasAttribute(volume->RootDirectory,FAT_DIR_ATTR_VOLUME_ID))
    {
        printf("FatVolume: Error - Root Directory does not have VOLUME_ID attribute\n");
        return false;
    }

    char vol_id[9];
    memset(vol_id,0,9);
    memcpy(vol_id,volume->RootDirectory->Name,8);
    printf("FatVolume: Found Volume ID %s\n", vol_id);

    return true;
}

void FatVolume_Destructor(FatVolume* part)
{
    printf("FatVolume Destructor\n");
    Memory_Free(part);
}

uint32_t FatVolume_GetFirstSectorOfCluster
(uint32_t cluster, uint32_t sectors_per_cluster, uint32_t first_data_sector)
{
    return first_data_sector + ((cluster - 2) * sectors_per_cluster);
}

void FatVolume_DebugSector(uint8_t* sector)
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
void FatVolume_ParseEntriesInCluster(uint8_t* cluster)
{
    
}