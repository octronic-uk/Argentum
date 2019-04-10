#include "FatVolume.h"

#include <stdio.h>
#include <string.h>

#include <Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>
#include <Drivers/PS2/PS2Driver.h>

#include "../MBR/MBR.h"
#include "FatDirectoryCluster.h"
#include "FatLfnCluster.h"

bool FatVolume_Constructor(struct FatVolume* self, struct Kernel* kernel, uint8_t ata_device_id, 
    uint8_t partition_id, uint32_t lba_first_sector, uint32_t sector_count) 
{
    printf("FatVolume: Constructor\n");

    self->Debug = 0;
    self->Kernel = kernel;
    self->AtaDeviceId = ata_device_id;
    self->PartitionId = partition_id;
    self->FirstSectorNumber = lba_first_sector;
    self->SectorCount = sector_count;

    if (self->Debug)
    {
        printf("FatVolume: FirstSector: 0x%x, SectorCount: 0x%x\n", self->FirstSectorNumber, self->SectorCount);
    }

    if (!FatVolume_ReadBPB(self))
    {
        return false;
    }

    return true;
}

bool FatVolume_ReadBPB(struct FatVolume* self)
{
    // Read the BPB from first sector of partition

    if (self->Debug) 
    {
        printf("FatVolume: Reading BPB...\n");
    }
    if (!FatVolume_ReadSector(self,0,(uint8_t*)&self->BiosParameterBlock))
    {
        printf("FatVolume: ATA error reading BPB\n");
        PS2Driver_WaitForKeyPress("Fat Volume Pause");
        return false;
    }

    if (self->Debug)
    {
        FatBPB_Debug(&self->BiosParameterBlock);
        FatBPB_DebugEBR16((struct FatExtendedBootRecord16*)&self->BiosParameterBlock.ExtendedBootRecord[0]);
    }

    self->RootDirSectorNumber = FatBPB_GetFirstRootDirectorySector(&self->BiosParameterBlock);
    if (self->Debug)
    {
        printf("FatVolume: Root Directory Sector at 0x%x\n", self->RootDirSectorNumber);
        PS2Driver_WaitForKeyPress("Fat Volume Pause");
    }
    return true;
}

void FatVolume_Destructor(struct FatVolume* self)
{
    printf("FatVolume: Destructor\n");
}

uint32_t FatVolume_GetFirstSectorOfCluster(struct FatVolume* self, uint32_t cluster)
{
    return FatBPB_GetFirstDataSector(&self->BiosParameterBlock) + ((cluster - 2) * self->BiosParameterBlock.SectorsPerCluster);
}

void FatVolume_DebugSector(uint8_t* sector)
{
    int i;
    printf("----------------------------------------");
    for (i=0; i<FAT_SECTOR_SIZE/4; i++)
    {
        if (i % 16 == 0)
        {
            printf("\n0x%02x: ",i);
        }
        printf("%02x ",sector[i]);
    }
    printf("\n----------------------------------------\n");
}

bool FatVolume_ReadSector(struct FatVolume* self, uint32_t sector_to_read, uint8_t* buffer)
{
    // Read the BPB from first sector of partition
    uint32_t physical_sector = self->FirstSectorNumber + sector_to_read;
    if (self->Debug) 
    {
        printf("FatVolume: Reading volume sector 0x%x (Physical sector 0x%x)\n",sector_to_read, physical_sector);
    }
    uint8_t ata_error = ATADriver_IDEAccess(
        &self->Kernel->ATA,
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