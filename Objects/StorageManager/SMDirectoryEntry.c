#include "SMDirectoryEntry.h"

#include <stdio.h>
#include <string.h>
#include <Objects/Kernel/Kernel.h>
#include <Objects/StorageManager/SMVolume.h>
#include <Objects/FAT16/FatVolume.h>

extern struct Kernel _Kernel;

bool SMDirectoryEntry_Constructor(struct SMDirectoryEntry* self, struct SMVolume* volume, uint16_t cluster, uint8_t fat_attributes)
{
    memset(self,0,sizeof(struct SMDirectoryEntry));
    self->Volume = volume;
    self->FatAttributes = fat_attributes;
    LinkedList_Constructor(&self->ClusterList);

    if (self->FatAttributes == FAT_DIR_ATTR_ARCHIVE)
    {
        uint32_t first_sector = FatVolume_GetFirstSectorOfCluster(&self->Volume->FatVolume, cluster);
        printf("SMDirectoryEntry: This entry is an archive\n"); 
        printf("\tFirst cluster number 0x%x\n",cluster);
        printf("\tFirst sector 0x%x\n", first_sector);
        enum FatType type = FatVolume_GetFatType(&volume->FatVolume);
        SMDirectoryEntry_PopulateClusterList(self, type, cluster);
        return true;
    }
    // Not necessary for files
    else if (self->FatAttributes == FAT_DIR_ATTR_DIRECTORY)
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

    printf("SMDirectoryEntry: Error - Unknown Attrubutes 0x%x\n",self->FatAttributes);
    return false;
}

void SMDirectoryEntry_Destructor(struct SMDirectoryEntry* self)
{
    LinkedList_FreeAllData(&self->ClusterList);
    LinkedList_Destructor(&self->ClusterList);
    FatDirectoryListing_Destructor(&self->FatListing);
}

bool SMDirectoryEntry_IsFile(struct SMDirectoryEntry* self)
{
    return (self->FatAttributes == FAT_DIR_ATTR_ARCHIVE);
}

void SMDirectoryEntry_PopulateClusterList(struct SMDirectoryEntry* self, enum FatType type, uint16_t first_cluster)
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


void SMDirectoryEntry_PopulateClusterListFat12(struct SMDirectoryEntry* self, uint16_t first_cluster) 
{

}

void SMDirectoryEntry_PopulateClusterListFat16(struct SMDirectoryEntry* self, uint16_t first_cluster) 
{
    uint16_t cluster_index = first_cluster;
    LinkedList_Constructor(&self->ClusterList);

    do
    {
        uint16_t* tmp = (uint16_t*)MemoryDriver_Allocate(&_Kernel.Memory,sizeof(uint16_t)); 
        *tmp = cluster_index;
        LinkedList_PushBack(&self->ClusterList, tmp);
        uint16_t new_index = ((uint16_t*)self->Volume->FatVolume.FileAllocationTableData)[cluster_index];
        cluster_index = new_index;
        printf("SMDirectoryEntry: Pushed back cluster value 0x%x\n",cluster_index);
    }
    while (cluster_index != FAT_END_OF_CHAIN_CLUSTER && cluster_index != FAT_BAD_CLUSTER);

    if (cluster_index == FAT_BAD_CLUSTER)
    {
        printf("SMDirectoryIndex: Error - Found a bad cluster\n");
    }
    else if (cluster_index == FAT_END_OF_CHAIN_CLUSTER)
    {
        printf("SMDirectoryIndex: Success - Found end of cluster chain sz=%d\n",LinkedList_Size(&self->ClusterList));
    }
}