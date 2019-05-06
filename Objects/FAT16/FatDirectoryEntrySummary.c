#include "FatDirectoryEntrySummary.h"

#include <string.h>
#include <Drivers/PS2/PS2Driver.h>

bool FatDirectoryEntrySummary_Constructor(
    struct FatDirectoryEntrySummary* self, 
    struct FatVolume* volume, 
    const char* name, 
    uint32_t first_sector,
    uint32_t first_cluster,
    uint8_t attributes,
    uint32_t file_size
)
{
    memset(self,0,sizeof(struct FatDirectoryEntrySummary));
    self->Volume = volume;
    self->Attributes = attributes;
    self->FirstSector = first_sector;
    self->FirstCluster = first_cluster;
    self->FileSize = file_size;
    memcpy(self->Name, name, FAT_LFN_NAME_SIZE);
    return true;
}

void FatDirectoryEntrySummary_Debug(struct FatDirectoryEntrySummary* self)
{
    printf("FatDirectoryEntrySummary:\n", self->Name);
    printf("\tFirst Cluster: 0x%x\n",self->FirstCluster);
    printf("\tFirst Sector:  0x%x\n",self->FirstSector);
}
