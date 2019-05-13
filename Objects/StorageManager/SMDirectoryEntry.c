#include "SMDirectoryEntry.h"

#include <stdio.h>
#include <string.h>
#include <Objects/Kernel/Kernel.h>
#include "SMVolume.h"
#include "FAT/FatVolume.h"

extern Kernel _Kernel;

bool SMDirectoryEntry_Constructor(SMDirectoryEntry* self, SMVolume* volume, FatDirectoryEntrySummary* fdEntry)
{
    printf("SMDirectoryEntry: Constructing\n");
    memset(self,0,sizeof(SMDirectoryEntry));
    self->Debug = true;
    self->Volume = volume;
    memcpy(&self->FatDirectoryEntrySummary, fdEntry, sizeof(FatDirectoryEntrySummary));
    LinkedList_Constructor(&self->ClusterList);
    uint16_t cluster = self->FatDirectoryEntrySummary.FirstCluster;
    uint8_t fatAttributes = self->FatDirectoryEntrySummary.Attributes;

    if (fatAttributes == FAT_DIR_ATTR_ARCHIVE)
    {
        uint32_t first_sector = FatVolume_GetFirstSectorOfCluster(&self->Volume->FatVolume, cluster);
        self->FirstSector = first_sector;
        if (self->Debug)
        {
            printf("SMDirectoryEntry: This entry is an archive\n"); 
            printf("\tFirst cluster number 0x%x\n",cluster);
            printf("\tFirst sector 0x%x\n", first_sector);
        }
        enum FatType type = FatVolume_GetFatType(&volume->FatVolume);
        SMDirectoryEntry_PopulateClusterList(self, type, cluster);
        return true;
    }
    // Not necessary for files
    else if (fatAttributes == FAT_DIR_ATTR_DIRECTORY)
    {
        uint8_t  sector_data[FAT_SECTOR_SIZE];
        uint32_t sector = FatVolume_GetFirstSectorOfCluster(&volume->FatVolume,cluster);

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
    LinkedList_FreeAllData(&self->ClusterList);
    LinkedList_Destructor(&self->ClusterList);
    FatDirectoryListing_Destructor(&self->FatListing);
}

bool SMDirectoryEntry_IsFile(SMDirectoryEntry* self)
{
    return (self->FatDirectoryEntrySummary.Attributes == FAT_DIR_ATTR_ARCHIVE);
}

void SMDirectoryEntry_PopulateClusterList(SMDirectoryEntry* self, enum FatType type, uint16_t first_cluster)
{
    switch(type)
    {
        case FAT_12:
            SMDirectoryEntry_PopulateClusterListFat12(self, first_cluster);
            break;
        case FAT_16:
            SMDirectoryEntry_PopulateClusterListFat16(self, first_cluster);
            break;
        default:
            break;
    }
}


/*
    If n is even, then the physical location of the entry is the low four bits in location 1+(3*n)/2 
    and the 8 bits in location (3*n)/2

    If n is odd, then the physical location of the entry is the high four bits in location (3*n)/2 
    and the 8 bits in location 1+(3*n)/2 
*/
void SMDirectoryEntry_PopulateClusterListFat12(SMDirectoryEntry* self, uint16_t first_cluster) 
{
    uint16_t cluster_index = first_cluster;
    LinkedList_Constructor(&self->ClusterList);

    do
    {
        uint16_t* tmp = (uint16_t*)MemoryDriver_Allocate(&_Kernel.Memory,sizeof(uint16_t)); 
        *tmp = cluster_index;
        LinkedList_PushBack(&self->ClusterList, tmp);
        //if(self->Debug) printf("SMDirectoryEntry: Pushed back cluster value 0x%x\n",*tmp);

        // Calculate next
        uint16_t new_index = 0;

        // Even
        if (cluster_index%2 == 0) 
        {
            uint16_t low_4_addr = 1 + ((3*cluster_index) / 2);
            uint8_t  low_4_value = self->Volume->FatVolume.FileAllocationTableData[low_4_addr] & 0x0F;
            uint16_t high_8_addr = (3*cluster_index) / 2;
            uint8_t  high_8_value = self->Volume->FatVolume.FileAllocationTableData[high_8_addr];
            //if(self->Debug) printf("SMDirectoryEnrty: 0x%x - Even - Low 4 at 0x%x is (0x%x) High 8 at 0x%x is (0x%x)\n",cluster_index, low_4_addr, low_4_value, high_8_addr, high_8_value);

            new_index  = low_4_value << 8;
            new_index |= high_8_value;
            //if(self->Debug) printf("\tnew_index = 0x%x\n",new_index);
        }
        // Odd
        else
        {
            uint16_t high_4_addr = (3*cluster_index) / 2;
            uint8_t  high_4_value = self->Volume->FatVolume.FileAllocationTableData[high_4_addr] & 0xF0;
            uint16_t low_8_addr = 1 + ((3*cluster_index) / 2);
            uint8_t  low_8_value = self->Volume->FatVolume.FileAllocationTableData[low_8_addr];

            //if(self->Debug) printf("SMDirectoryEnrty: 0x%x - Odd - Low 8 at 0x%x is (0x%x) High 4 at 0x%x is (0x%x)\n",cluster_index, low_8_addr, low_8_value, high_4_addr, high_4_value);

            new_index  = high_4_value >> 4;
            new_index |= low_8_value << 4;
            //if(self->Debug) printf("\tnew_index = 0x%x\n",new_index);
        }
        cluster_index = new_index;
    }
    while (cluster_index != FAT_END_OF_CHAIN_CLUSTER_12 && cluster_index != FAT_BAD_CLUSTER_12);

    if (cluster_index == FAT_BAD_CLUSTER_12)
    {
        if(self->Debug) printf("SMDirectoryIndex: Error - Found a bad cluster\n");
    }
    else if (cluster_index == FAT_END_OF_CHAIN_CLUSTER_12)
    {
        if(self->Debug)printf("SMDirectoryIndex: Success - Found end of cluster chain sz=%d\n",LinkedList_Size(&self->ClusterList));
    }
}

void SMDirectoryEntry_PopulateClusterListFat16(SMDirectoryEntry* self, uint16_t first_cluster) 
{
    uint16_t cluster_index = first_cluster;
    LinkedList_Constructor(&self->ClusterList);

    do
    {
        uint16_t* tmp = (uint16_t*)MemoryDriver_Allocate(&_Kernel.Memory,sizeof(uint16_t)); 
        *tmp = cluster_index;
        LinkedList_PushBack(&self->ClusterList, tmp);
        //if(self->Debug) printf("SMDirectoryEntry: Pushed back cluster value 0x%x\n",*tmp);
        uint16_t new_index = ((uint16_t*)self->Volume->FatVolume.FileAllocationTableData)[cluster_index];
        cluster_index = new_index;
    }
    while (cluster_index < FAT_END_OF_CHAIN_CLUSTER_16 && cluster_index != FAT_BAD_CLUSTER_16);

    if (cluster_index == FAT_BAD_CLUSTER_16)
    {
        if(self->Debug) printf("SMDirectoryIndex: Error - Found a bad cluster\n");
    }
    else if (cluster_index >= FAT_END_OF_CHAIN_CLUSTER_16)
    {
        if(self->Debug) printf("SMDirectoryIndex: Success - Found end of cluster chain sz=%d\n",LinkedList_Size(&self->ClusterList));
    }
}

uint32_t SMDirectoryEntry_SetFileOffset(SMDirectoryEntry* self, uint32_t offset)
{
    uint32_t seek_max = self->FatDirectoryEntrySummary.FileSize-1; 
    if (offset > seek_max) return seek_max;

    uint32_t sectors_per_cluster = self->Volume->FatVolume.BiosParameterBlock.SectorsPerCluster;
    uint32_t sector_size = self->Volume->FatVolume.BiosParameterBlock.BytesPerSector;
    uint32_t offset_in_sectors = offset / sector_size;
    uint32_t cluster = offset_in_sectors / sectors_per_cluster;
    uint32_t sector_in_cluster = offset_in_sectors % sectors_per_cluster;
    uint32_t byte_offset_in_sector = offset % sector_size;

    self->FileOffset.ByteOffset = offset;
    self->FileOffset.SectorsPerCluster = sectors_per_cluster,
    self->FileOffset.SectorSize = sector_size,
    self->FileOffset.OffsetInSectors = offset_in_sectors,
    self->FileOffset.Cluster = cluster,
    self->FileOffset.SectorInCluster = sector_in_cluster,
    self->FileOffset.ByteOffsetIntoSector = byte_offset_in_sector;
    if (self->Debug) SMFileOffset_Debug(&self->FileOffset);
    return offset;
}

uint32_t SMDirectoryEntry_Read(SMDirectoryEntry* self, uint32_t size, uint8_t* buffer)
{
    uint32_t bytes_per_sector = self->Volume->FatVolume.BiosParameterBlock.BytesPerSector;
    uint32_t sectors_required = (size/bytes_per_sector) + 1;
    uint32_t bytes_required = sectors_required * bytes_per_sector;
    uint8_t* tmp_buffer = MemoryDriver_Allocate(&_Kernel.Memory, bytes_required);
    uint32_t tmp_buffer_offset = 0;
    memset(tmp_buffer, 0, bytes_required);
    uint32_t i;
    uint32_t cluster_index = self->FileOffset.Cluster;

    for (i=0; i<sectors_required; i++)
    {
        tmp_buffer_offset = i * bytes_per_sector;
        uint16_t cluster = *((uint16_t*)LinkedList_At(&self->ClusterList, cluster_index));
        uint16_t  sector_in_cluster = (self->FileOffset.SectorInCluster + i) % self->FileOffset.SectorsPerCluster;
        if (self->Debug) printf("SMDirectoryEntry: Cluster %d (0x%x) Sector %d,\n", cluster_index, cluster, sector_in_cluster);

        if(FatVolume_ReadSector(
            &self->Volume->FatVolume, 
            self->FirstSector + self->FileOffset.OffsetInSectors,
            &tmp_buffer[tmp_buffer_offset]))
        {
            if(self->Debug) printf("SMDirectoryEntry: Reading file data...\n");
            if (sector_in_cluster == self->FileOffset.SectorsPerCluster-1)
            {
                cluster_index++;
            }
        }
        else
        {
            printf("SMDirectoryEntry: Error whilie Reading file data...\n");
            MemoryDriver_Free(&_Kernel.Memory, tmp_buffer);
            return false;
        }
    }
    memcpy(buffer, &tmp_buffer[self->FileOffset.ByteOffsetIntoSector], size);
    MemoryDriver_Free(&_Kernel.Memory, tmp_buffer);
    return true;
}

uint32_t SMDirectoryEntry_Write(SMDirectoryEntry* self, uint32_t size, uint8_t* buffer)
{
    return 0;
}