#include "FatVolume.h"

#include <stdio.h>
#include <string.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>
#include "../RamDisk/RamDisk.h"
#include "../MBR/MBR.h"
#include "FatDirectoryEntryData.h"
#include "FatLfnCluster.h"
#include <Objects/LinkedList/LinkedList.h>

extern Kernel _Kernel;

bool FatVolume_ATAConstructor(FatVolume* self,  uint8_t ata_device_index, uint8_t partition_index, uint32_t lba_first_sector, uint32_t sector_count) 
{
    printf("FatVolume: ATA Constructor\n");
    memset(self,0,sizeof(FatVolume));
    self->Debug = false;
    self->AtaDeviceIndex = ata_device_index;
    self->PartitionIndex = partition_index;
    self->FloppyDeviceIndex = -1;
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

bool FatVolume_FloppyConstructor(FatVolume* self,  uint8_t device_index) 
{
    printf("FatVolume: Floppy Constructor\n");
    memset(self,0,sizeof(FatVolume));
    self->Debug = false;
    self->AtaDeviceIndex = -1;
    self->PartitionIndex = -1;
    self->FloppyDeviceIndex = device_index;

    self->FirstSectorNumber = 0;
    self->SectorCount = 20480; // FLOPPY_144_TOTAL_SECTORS;

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

bool FatVolume_RamDiskConstructor(FatVolume* self,  uint8_t device_index) 
{
    printf("FatVolume: RamDisk Constructor\n");
    memset(self,0,sizeof(FatVolume));
    self->Debug = false;
    self->AtaDeviceIndex = -1;
    self->PartitionIndex = -1;
    self->FloppyDeviceIndex = -1;
    self->RamDiskIndex = device_index;
    self->FirstSectorNumber = 0;
    self->SectorCount = 0;

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

void FatVolume_Destructor(FatVolume* self)
{
    MemoryDriver_Free(&_Kernel.Memory, self->FileAllocationTableData);
}

bool FatVolume_ReadBPB(FatVolume* self)
{
    // Read the BPB from first sector of partition

    if (self->Debug) 
    {
        printf("FatVolume: Reading BPB...\n");
    }

    if (!FatVolume_ReadSector(self,0,(uint8_t*)&self->BiosParameterBlock))
    {
        printf("FatVolume: ATA error reading BPB\n");
        return false;
    }

    if (self->Debug)
    {
        FatBPB_Debug(&self->BiosParameterBlock);
        FatBPB_DebugEBR16((FatExtendedBootRecord16*)&self->BiosParameterBlock.ExtendedBootRecord[0]);
    }

    self->RootDirSectorNumber = FatBPB_GetFirstRootDirectorySector(&self->BiosParameterBlock);

    if (self->Debug)
    {
        printf("FatVolume: Root Directory Sector at 0x%x\n", self->RootDirSectorNumber);
    }
    return true;
}

uint32_t FatVolume_GetFirstSectorOfCluster(FatVolume* self, uint32_t cluster)
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

bool FatVolume_WriteSector(FatVolume* self, uint32_t sector_to_write, uint8_t* buffer)
{
    uint32_t physical_sector = self->FirstSectorNumber + sector_to_write;
    if (self->Debug) 
    {
        printf("FatVolume: Reading volume sector 0x%x (Physical sector 0x%x)\n",sector_to_write, physical_sector);
    }

    // Write to an ATA disk
    if (self->AtaDeviceIndex >= 0 && self->PartitionIndex >= 0)
    {
        uint8_t ata_error = ATADriver_IDEAccess(&_Kernel.ATA, ATA_WRITE, self->AtaDeviceIndex, physical_sector, 1, (void*)buffer);

        if (ata_error)
        {
            printf("FatVolume: Error - ATA Error writing to sector %d.\n",physical_sector);
            return false;
        }
    } 
    // Write to Floppy disk
    else if (self->FloppyDeviceIndex >= 0)
    {
        if (!FloppyDriver_WriteSectorLBA(&_Kernel.Floppy, physical_sector, buffer))
        {
            printf("FatVolume: Error - Floppy Error reading sector %d.\n",physical_sector);
            return false;
        }
    }
    // Write to RamDisk 
    else if (self->RamDiskIndex >= 0)
    {
        RamDisk* rd = (RamDisk*)LinkedList_At(&_Kernel.StorageManager.RamDisks,self->RamDiskIndex);
        if (!RamDisk_WriteSectorLBA(rd, physical_sector,buffer))
        {
            printf("FatVolume: Error - RamDisk Error writing sector %d.\n",physical_sector);
            return false;
        }
    }
    return true;
}

bool FatVolume_ReadSector(FatVolume* self, uint32_t sector_to_read, uint8_t* buffer)
{
    // Read the BPB from first sector of partition
    uint32_t physical_sector = self->FirstSectorNumber + sector_to_read;
    if (self->Debug) 
    {
        printf("FatVolume: Reading volume sector 0x%x (Physical sector 0x%x)\n",sector_to_read, physical_sector);
    }

    // Read from ATA disk
    if (self->AtaDeviceIndex >= 0 && self->PartitionIndex >= 0)
    {
        uint8_t ata_error = ATADriver_IDEAccess(&_Kernel.ATA, ATA_READ, self->AtaDeviceIndex, physical_sector, 1, (void*)buffer);

        if (ata_error)
        {
            printf("FatVolume: Error - ATA Error reading sector %d.\n",physical_sector);
            return false;
        }
    } 
    // Read from Floppy disk
    else if (self->FloppyDeviceIndex >= 0)
    {
        if (!FloppyDriver_ReadSectorLBA(&_Kernel.Floppy, physical_sector, buffer))
        {
            printf("FatVolume: Error - Floppy Error reading sector %d.\n",physical_sector);
            return false;
        }
    }
    // Read from RamDisk 
    else if (self->RamDiskIndex >= 0)
    {
        RamDisk* rd = (RamDisk*)LinkedList_At(&_Kernel.StorageManager.RamDisks,self->RamDiskIndex);
        if (!RamDisk_ReadSectorLBA(rd, physical_sector,buffer))
        {
            printf("FatVolume: Error - RamDisk Error reading sector %d.\n",physical_sector);
            return false;
        }
    }
    return true;
}


enum FatType FatVolume_GetFatType(FatVolume* self)
{
    return FatBPB_GetFATType(&self->BiosParameterBlock);
}

bool FatVolume_ReadFileAllocationTableData(FatVolume* self)
{
    if (self->Debug) printf("FatVolume: Reading File Allocation Table\n");
    uint32_t fat_start_sector = self->BiosParameterBlock.ReservedSectorCount;
    uint32_t fat_size_in_sectors = self->BiosParameterBlock.TableSize16;
    uint32_t data_size = fat_size_in_sectors * FAT_SECTOR_SIZE;
    self->FileAllocationTableData = MemoryDriver_Allocate(&_Kernel.Memory, data_size);
    memset(self->FileAllocationTableData,0,data_size);

    if (self->FileAllocationTableData)
    {
        uint32_t i;
        for (i=0; i<fat_size_in_sectors; i++)
        {
            if (self->Debug) printf("FatVolume: Reading FAT Table sector 0x%x (physical 0x%x)\n",i, fat_start_sector+i);
            if (!FatVolume_ReadSector(self, fat_start_sector + i, &self->FileAllocationTableData[i*FAT_SECTOR_SIZE]))
            {
                printf("FatVolume: Error reading FileAllocationTable\n");
                MemoryDriver_Free(&_Kernel.Memory, self->FileAllocationTableData);
                return false;
            }
        }
    }
    return true;
}