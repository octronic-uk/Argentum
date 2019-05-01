#include "FatVolume.h"

#include <stdio.h>
#include <string.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include <Drivers/PIT/PITDriver.h>

#include <Objects/MBR/MBR.h>
#include <Objects/FAT16/FatDirectoryEntryData.h>
#include <Objects/FAT16/FatLfnCluster.h>

extern struct Kernel _Kernel;

bool FatVolume_ATAConstructor(struct FatVolume* self,  uint8_t ata_device_id, uint8_t partition_id, uint32_t lba_first_sector, uint32_t sector_count) 
{
    printf("FatVolume: ATA Constructor\n");
    self->Debug = false;

    self->AtaDeviceId = ata_device_id;
    self->PartitionId = partition_id;
    self->FloppyDeviceId = -1;

    self->FirstSectorNumber = lba_first_sector;
    self->SectorCount = sector_count;

    if (self->Debug)
    {
        printf("FatVolume: FirstSector: 0x%x, SectorCount: 0x%x\n", self->FirstSectorNumber, self->SectorCount);
        PS2Driver_WaitForKeyPress("");
    }

    if (!FatVolume_ReadBPB(self))
    {
        return false;
    }

    return true;
}

bool FatVolume_FloppyConstructor(struct FatVolume* self,  uint8_t device_id) 
{
    printf("FatVolume: Floppy Constructor\n");
    self->Debug = false;

    self->AtaDeviceId = -1;
    self->PartitionId = -1;
    self->FloppyDeviceId = device_id;

    self->FirstSectorNumber = 0;
    self->SectorCount = FLOPPY_144_TOTAL_SECTORS;

    if (self->Debug)
    {
        printf("FatVolume: FirstSector: 0x%x, SectorCount: 0x%x\n", self->FirstSectorNumber, self->SectorCount);
        PS2Driver_WaitForKeyPress("");
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
        PS2Driver_WaitForKeyPress("");
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
    PS2Driver_WaitForKeyPress("");
}

bool FatVolume_WriteSector(struct FatVolume* self, uint32_t sector_to_write, uint8_t* buffer)
{
    printf("FatVolume: Error - WRITE IS NOT YET IMPLEMENTED\n");
    PS2Driver_WaitForKeyPress("WRITE ERROR");
}

bool FatVolume_ReadSector(struct FatVolume* self, uint32_t sector_to_read, uint8_t* buffer)
{
    // Check the sector cache
    uint32_t index;
    for (index=0; index < FAT_CACHED_SECTORS_SIZE; index++)
    {
        struct FatCachedSector* next = &self->SectorCache[index];
        if (next->SectorNumber == sector_to_read && next->InUse && !next->Invalid)
        {
            if (self->Debug)
            {
                printf("FatVolume: Found cached sector 0x%x\n",sector_to_read);
                PS2Driver_WaitForKeyPress("");
            }
            memcpy(buffer,next->Data,FAT_SECTOR_SIZE);
            next->LastAccess = _Kernel.PIT.Ticks;

            if (self->Debug) 
            {
                FatCachedSector_Debug(next);
            }
            return true;
        }
    }

    // Read the BPB from first sector of partition
    uint32_t physical_sector = self->FirstSectorNumber + sector_to_read;
    if (self->Debug) 
    {
        printf("FatVolume: Reading volume sector 0x%x (Physical sector 0x%x)\n",sector_to_read, physical_sector);
        PS2Driver_WaitForKeyPress("");
    }

    // Read from ATA disk
    if (self->AtaDeviceId >= 0 && self->PartitionId >= 0)
    {
        uint8_t ata_error = ATADriver_IDEAccess(&_Kernel.ATA, ATA_READ, self->AtaDeviceId, physical_sector, 1, (void*)buffer);

        if (ata_error)
        {
            printf("FatVolume: Error - ATA Error reading sector %d.\n",physical_sector);
            PS2Driver_WaitForKeyPress("");
            return false;
        }
    } 
    // Read from Floppy disk
    else if (self->FloppyDeviceId >= 0)
    {
        if (!FloppyDriver_ReadSectorLBA(&_Kernel.Floppy, physical_sector))
        {
            printf("FatVolume: Error - Floppy Error reading sector %d.\n",physical_sector);
            PS2Driver_WaitForKeyPress("");
            return false;
        }
        memcpy(buffer, FloppyDriver_GetDMABuffer(&_Kernel.Floppy), FAT_SECTOR_SIZE);
    }

    // Insert sector into cache
    if (self->Debug) 
    {
        printf("FatVolume: Caching sector 0x%x\n",sector_to_read);
        PS2Driver_WaitForKeyPress("");
    }
    struct FatCachedSector* cached = FatVolume_GetNextAvailableCachedSector(self);
    if (cached)
    {
        FatCachedSector_Constructor(cached, self, sector_to_read);
        memcpy(cached->Data,buffer,FAT_SECTOR_SIZE);

        if(self->Debug) 
        {
            FatCachedSector_Debug(cached);
        }
    } 
    else
    {
        printf("FatVolume: Error - Next Cache Block returned NULL??\n");
        PS2Driver_WaitForKeyPress("FatVolume Error");
        return false;
    }
    
    return true;
}

struct FatCachedSector* FatVolume_GetNextAvailableCachedSector(struct FatVolume* volume)
{
    struct FatCachedSector* oldest = 0;

    // Find a free block

    uint32_t index;
    for (index=0; index < FAT_CACHED_SECTORS_SIZE; index++)
    {
        struct FatCachedSector* next = &volume->SectorCache[index];
        // This block is not being used
        if (!next->InUse) 
        {
            if (volume->Debug)
            {
                printf("FatVolume: Using not-in-use sector cache block %d\n",index);
                PS2Driver_WaitForKeyPress("");
            }
            return next;
        } 
        // The block was written to and is now invalid, so reusable
        else if (next->Invalid)
        {
            if (volume->Debug)
            {
                printf("FatVolume: Using invalid sector cache block %d\n",index);
                PS2Driver_WaitForKeyPress("");
            }
            return next; 
        }

        // Check if this is the oldest block
        if (!oldest)
        {
            oldest = next;
        }
        else if (next->LastAccess < oldest->LastAccess)
        {
            oldest = next;
        }
    } 

    // Use the oldest block
    return oldest;
}