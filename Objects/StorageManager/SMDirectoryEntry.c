#include "SMDirectoryEntry.h"

#include <stdio.h>
#include <string.h>
#include "SMVolume.h"
#include "../FAT16/FatVolume.h"
#include <Drivers/PS2/PS2Driver.h>

bool SMDirectoryEntry_Constructor(struct SMDirectoryEntry* self, struct SMVolume* volume, uint32_t cluster, uint8_t fat_attributes)
{
    memset(self,0,sizeof(struct SMDirectoryEntry));
    self->Volume = volume;
    self->FatAttributes = fat_attributes;

    if (self->FatAttributes == FAT_DIR_ATTR_ARCHIVE)
    {
        uint32_t first_sector = FatVolume_GetFirstSectorOfCluster(&self->Volume->FatVolume, cluster);
        printf("SMDirectoryEntry: This is an archive\n"); 
        printf("\tFirst cluster number 0x%x\n",cluster);
        printf("\tFirst sector 0x%x\n", first_sector);
        PS2Driver_WaitForKeyPress("Archive cluster");
        return true;
    }
    // Not necessary for files
    else if (self->FatAttributes == FAT_DIR_ATTR_DIRECTORY)
    {
        uint8_t sector_data[FAT_SECTOR_SIZE];
        uint32_t sector = FatVolume_GetFirstSectorOfCluster(&volume->FatVolume,cluster);

        if(FatVolume_ReadSector(&volume->FatVolume, sector, sector_data))
        {
            if (FatTableListing_Constructor(&self->FatListing, &self->Volume->FatVolume, sector_data))
            {
                return true;     
            } 
        }
    }

    printf("SMDirectoryEntry: Error - Unknown Attrubutes 0x%x\n",self->FatAttributes);
    PS2Driver_WaitForKeyPress("SMDirectoryEntry Error");
    return false;
}

void SMDirectoryEntry_Destructor(struct SMDirectoryEntry* directory)
{

}

bool SMDirectoryEntry_IsFile(struct SMDirectoryEntry* self)
{
    return (self->FatAttributes == FAT_DIR_ATTR_ARCHIVE);
}