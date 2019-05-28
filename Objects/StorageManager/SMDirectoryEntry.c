#include "SMDirectoryEntry.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Objects/Kernel/Kernel.h>
#include "SMVolume.h"
#include "FAT/FatVolume.h"

#define min(x,y) x < y ? x : y

extern Kernel _Kernel;

bool SMDirectoryEntry_Constructor(SMDirectoryEntry* self, SMVolume* volume, FatDirectoryEntrySummary* fdEntry)
{
    printf("SMDirectoryEntry: Constructing\n");
    memset(self,0,sizeof(SMDirectoryEntry));
    self->Debug = false;
    self->Volume = volume;
    memcpy(&self->FatDirectoryEntrySummary, fdEntry, sizeof(FatDirectoryEntrySummary));

    self->FirstCluster = self->FatDirectoryEntrySummary.FirstCluster;
    uint8_t fatAttributes = self->FatDirectoryEntrySummary.Attributes;

    if (fatAttributes == FAT_DIR_ATTR_ARCHIVE)
    {
        self->FirstCluster = self->FirstCluster;
        if (self->Debug)
        {
            printf("SMDirectoryEntry: This entry is an archive\n"); 
            printf("\tFirst cluster number 0x%x\n",self->FirstCluster);
        }
        self->FatType = FatVolume_GetFatType(&volume->FatVolume);
        SMDirectoryEntry_SetFileOffset(self,0);
        return true;
    }
    // Not necessary for files
    else if (fatAttributes == FAT_DIR_ATTR_DIRECTORY)
    {
        uint8_t  sector_data[FAT_SECTOR_SIZE];
        uint32_t sector = FatVolume_GetFirstSectorOfCluster(&volume->FatVolume,self->FirstCluster);

        if(FatVolume_ReadSector(&volume->FatVolume, sector, sector_data))
        {
            if (FatDirectoryListing_Constructor(&self->FatListing, &self->Volume->FatVolume, sector_data, 1))
            {
                return true;     
            } 
        }
    }

    printf("SMDirectoryEntry: Error - Unknown Attrubutes 0x%x\n",fatAttributes);
    return false;
}

void SMDirectoryEntry_Destructor(SMDirectoryEntry* self)
{
    FatDirectoryListing_Destructor(&self->FatListing);
}

bool SMDirectoryEntry_IsFile(SMDirectoryEntry* self)
{
    return (self->FatDirectoryEntrySummary.Attributes == FAT_DIR_ATTR_ARCHIVE);
}

uint16_t SMDirectoryEntry_GetNextCluster(SMDirectoryEntry* self, uint16_t cluster)
{
    switch(self->FatType)
    {
        case FAT_12:
            return SMDirectoryEntry_GetNextClusterFat12(self, cluster);
        case FAT_16:
            return SMDirectoryEntry_GetNextClusterFat16(self, cluster);
    }
    return 0;
}


/*
    If n is even, then the physical location of the entry is the low four bits in location 1+(3*n)/2 
    and the 8 bits in location (3*n)/2

    If n is odd, then the physical location of the entry is the high four bits in location (3*n)/2 
    and the 8 bits in location 1+(3*n)/2 
*/
uint16_t SMDirectoryEntry_GetNextClusterFat12(SMDirectoryEntry* self, uint16_t cluster) 
{
    // Even
    if (cluster % 2 == 0) 
    {
        uint16_t low_4_addr = 1 + (( 3 * cluster) / 2);
        uint8_t  low_4_value = self->Volume->FatVolume.FileAllocationTableData[low_4_addr] & 0x0F;
        uint16_t high_8_addr = (3*cluster) / 2;
        uint8_t  high_8_value = self->Volume->FatVolume.FileAllocationTableData[high_8_addr];
        //if(self->Debug) printf("SMDirectoryEnrty: 0x%x - Even - Low 4 at 0x%x is (0x%x) High 8 at 0x%x is (0x%x)\n",cluster_index, low_4_addr, low_4_value, high_8_addr, high_8_value);
        uint16_t next_cluster = low_4_value << 8;
        next_cluster |= high_8_value;
        //if(self->Debug) printf("\tnew_index = 0x%x\n",new_index);
        return next_cluster;
    }
    // Odd
    else
    {
        uint16_t high_4_addr = (3 * cluster) / 2;
        uint8_t  high_4_value = self->Volume->FatVolume.FileAllocationTableData[high_4_addr] & 0xF0;
        uint16_t low_8_addr = 1 + ((3 * cluster) / 2);
        uint8_t  low_8_value = self->Volume->FatVolume.FileAllocationTableData[low_8_addr];
        //if(self->Debug) printf("SMDirectoryEnrty: 0x%x - Odd - Low 8 at 0x%x is (0x%x) High 4 at 0x%x is (0x%x)\n",cluster_index, low_8_addr, low_8_value, high_4_addr, high_4_value);
        uint16_t next_cluster  = high_4_value >> 4;
        next_cluster |= low_8_value << 4;
        //if(self->Debug) printf("\tnew_index = 0x%x\n",new_index);
        return next_cluster;
    }
    return 0;
}

uint16_t SMDirectoryEntry_GetNextClusterFat16(SMDirectoryEntry* self, uint16_t cluster) 
{
    return ((uint16_t*)self->Volume->FatVolume.FileAllocationTableData)[cluster];
}

uint32_t SMDirectoryEntry_SetFileOffset(SMDirectoryEntry* self, uint32_t offset)
{
    uint32_t seek_max = self->FatDirectoryEntrySummary.FileSize-1; 
    if (offset > seek_max) return seek_max;

    uint32_t sectors_per_cluster = self->Volume->FatVolume.BiosParameterBlock.SectorsPerCluster;
    uint32_t sector_size = self->Volume->FatVolume.BiosParameterBlock.BytesPerSector;
    uint32_t offset_in_sectors = offset / sector_size;
    uint32_t cluster_number = offset_in_sectors / sectors_per_cluster;
    uint32_t sector_in_cluster = offset_in_sectors % sectors_per_cluster;
    uint32_t byte_offset_in_sector = offset % sector_size;

    uint16_t cluster = self->FirstCluster;
    uint16_t i = 0;
    for (i=0; i<cluster_number; i++)
    {
       cluster = SMDirectoryEntry_GetNextCluster(self, cluster);
    }

    self->FileOffset.ByteOffset = offset;
    self->FileOffset.SectorsPerCluster = sectors_per_cluster,
    self->FileOffset.BytesPerSector = sector_size,
    self->FileOffset.Cluster = cluster,
    self->FileOffset.SectorOffsetInCluster = sector_in_cluster,
    self->FileOffset.ByteOffsetIntoSector = byte_offset_in_sector;
    if (self->Debug) SMFileOffset_Debug(&self->FileOffset);
    return offset;
}

uint32_t SMDirectoryEntry_Read(SMDirectoryEntry* self, uint32_t size, uint8_t* buffer)
{
    if(self->Debug) printf("SMDirectoryEntry: Reading file data...\n");
    uint32_t bytes_per_sector = self->FileOffset.BytesPerSector;
    uint32_t sectors_required = (size/bytes_per_sector) +1;
    uint32_t bytes_required = sectors_required * bytes_per_sector;
    uint32_t read_buffer_offset = 0;
    int32_t  bytes_remaining = size;
    uint32_t bytes_read = 0;

    uint8_t* sector_buffer = malloc(bytes_per_sector);
    memset(sector_buffer, 0, bytes_per_sector);

    uint8_t* read_buffer = malloc(bytes_required);
    memset(read_buffer, 0, bytes_required);

    uint32_t i;
    for (i=0; i<sectors_required; i++)
    {
        uint32_t target_sector = 
            FatVolume_GetFirstSectorOfCluster(&self->Volume->FatVolume, self->FileOffset.Cluster) + 
            self->FileOffset.SectorOffsetInCluster;

        if(self->Debug) printf("\nSMDirectoryEntry: Target Sector %d (0x%x)\n", target_sector, target_sector);

        if(FatVolume_ReadSector(&self->Volume->FatVolume, target_sector, sector_buffer))
        {
            bytes_read = bytes_per_sector - self->FileOffset.ByteOffsetIntoSector;
            if(self->Debug) printf("SMDirectoryEntry: Reading from byte %d in sector, so read %d\n",self->FileOffset.ByteOffsetIntoSector, bytes_read);
            if(self->Debug) printf("SMDirectoryEntry: bytes remaining was %d ... ",bytes_remaining);
            bytes_remaining -= bytes_read;
            if(self->Debug) printf("now %d\n",bytes_remaining);
            uint32_t bytes_to_copy = bytes_remaining < 0 ? bytes_read + bytes_remaining : bytes_read;
            if(self->Debug) printf("SMDirectoryEntry: copying %d bytes\n",bytes_to_copy);
            // Copy up to the amount of bytes remaining
            memcpy(
                &read_buffer[read_buffer_offset], 
                &sector_buffer[self->FileOffset.ByteOffsetIntoSector], 
                // if bytes_remaining is negative, copy bytes_read - bytes read over limit
               bytes_to_copy 
            );
            read_buffer_offset += bytes_to_copy;
            SMDirectoryEntry_SetFileOffset(self,self->FileOffset.ByteOffset+bytes_to_copy);
        }
        else
        {
            printf("SMDirectoryEntry: Error whilie Reading file data\n");
            free(read_buffer);
            free(sector_buffer);
            return 0;
        }
    }
    if (self->Debug) printf("SMDirectoryEntry: Read loop is done, freeing buffers\n");

    // Copy into the caller's buffer
    if(self->Debug) printf("SMDirectoryEntry: Copying into return buffer\n");
    memcpy(buffer, read_buffer, read_buffer_offset);

    if(self->Debug) printf("SMDirectoryEntry: Freeing sector buffer\n");
    free(sector_buffer);

    if(self->Debug) printf("SMDirectoryEntry: Freeing read buffer\n");
    free(read_buffer);
    if(self->Debug) printf("SMDirectoryEntry: Read is returning\n");
    return read_buffer_offset;
}

uint32_t SMDirectoryEntry_Write(SMDirectoryEntry* self, uint32_t size, uint8_t* buffer)
{
    return 0;
}