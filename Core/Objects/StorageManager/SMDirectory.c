#include "SMDirectory.h"

#include <stdio.h>
#include <string.h>
#include "SMVolume.h"
#include "FAT/FatVolume.h"

bool SMDirectory_Constructor(struct SMDirectory* self, struct SMVolume* volume, uint32_t cluster)
{
    memset(self,0,sizeof(struct SMDirectory));
    self->Volume = volume;

    uint8_t sector_data[FAT_SECTOR_SIZE];
    uint32_t sector = FatVolume_GetFirstSectorOfCluster(&volume->FatVolume,cluster);

    if(FatVolume_ReadSector(&volume->FatVolume, sector, sector_data))
    {
        if (FatDirectoryListing_Constructor(&self->FatListing, &self->Volume->FatVolume, sector_data))
        {
            return true;     
        } 
    }
    return false;
}

void SMDirectory_Destructor(struct SMDirectory* directory)
{

}

