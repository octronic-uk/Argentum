#include "FatDirectoryEntry.h"

#include <string.h>
#include <Drivers/PS2/PS2Driver.h>

bool FatDirectoryEntry_Constructor(
    struct FatDirectoryEntry* self, 
    struct FatVolume* volume, 
    struct FatDirectoryCluster* cluster, 
    const char* name, 
    uint32_t first_sector,
    uint32_t first_cluster
)
{
    memset(self,0,sizeof(struct FatDirectoryEntry));

    self->Volume = volume;
    memcpy(self->Name, name, FAT_LFN_NAME_SIZE);
    memcpy(&self->ClusterData, cluster, sizeof(struct FatDirectoryCluster));
    self->FirstSector = first_sector;
    self->FirstCluster = first_cluster;

}

void FatDirectoryEntry_Debug(struct FatDirectoryEntry* self)
{
    printf("FatDirectoryEntry:\n", self->Name);
    printf("\tFirst Cluster: 0x%x\n",self->FirstCluster);
    printf("\tFirst Sector:  0x%x\n",self->FirstSector);
    PS2Driver_WaitForKeyPress("FatDirectoryEntry Debug");
}
