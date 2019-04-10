#include <Objects/StorageManager/SMVolume.h>

#include <Kernel.h>
#include "FAT/FatVolume.h"
#include "SMDrive.h"
#include "SMDirectory.h"

#include <stdio.h>
#include <string.h>

bool SMVolume_Constructor(
    struct SMVolume* self, struct SMDrive* parent, struct Kernel* kernel, 
    uint8_t volume_index, uint32_t first_sector, uint32_t sectors_in_partition
)
{
    memset(self,0,sizeof(struct SMVolume));
    self->Debug = false;
    self->ParentDrive = parent;
    self->Kernel = kernel;
    self->VolumeIndex = volume_index;
    self->FirstSectorIndex = first_sector;
    self->SectorsInPartition = sectors_in_partition;

    if (FatVolume_Constructor(&self->FatVolume, self->Kernel, self->ParentDrive->AtaIndex, 
        self->VolumeIndex, self->FirstSectorIndex, self->SectorsInPartition))
    {
       return true;
    }
    printf("SMVolume: Error - Unable to construct corresponding FatVolume\n");
    return false;
}

bool SMVolume_GetDirectory(struct SMVolume* self, struct SMDirectory* dir, uint8_t* sector_buffer, struct SMPath* path)
{
    struct FatDirectoryListing fatListing;
    if (!FatDirectoryListing_Constructor(&fatListing, &self->FatVolume, sector_buffer))
    {
        printf("SMVolume: Error - Could not read directory listing\n");
        return false;
    }

    FatDirectoryListing_Debug(&fatListing);

    const char* dir_name = path->Directories[path->WalkIndex];
    // Compare Path name to FAT entries
    uint8_t fat_entry_index;
    for (
        fat_entry_index = 0; 
        fat_entry_index < fatListing.EntryCount; 
        fat_entry_index++
    )
    {
        struct FatDirectoryEntry* entry = &fatListing.Entries[fat_entry_index];
        char* entry_name = entry->Name;

        if (self->Debug)
        {
            printf("SMVolume: Comparing %s with target %s\n",entry_name,dir_name);
            PS2Driver_WaitForKeyPress("SMVolume Pause");
        }

        // Chech each entry in the listing 
        if (strncmp(entry_name, dir_name, FAT_LFN_NAME_SIZE) == 0)
        {
            // Last directory in path
            if (path->Directories[path->WalkIndex+1][0] == 0)
            {
                if (self->Debug)
                {
                    printf("SMVolume: Last Directory in path is %s\n",path->Directories[path->WalkIndex]);
                    PS2Driver_WaitForKeyPress("SMVolume Pause");
                }
                return SMDirectory_Constructor(dir,self,entry->FirstCluster);
            }
            // Recurse to next directory
            else
            {
                // Read directory
                uint8_t next_sector_buffer[FAT_SECTOR_SIZE];
                uint32_t sector = entry->FirstSector;
                if (self->Debug)
                {
                    printf("SMVolume: Reading first sector 0x%x, cluster 0x%x of next directory in path\n", sector, entry->FirstCluster);
                    PS2Driver_WaitForKeyPress("SMVolume Pause");
                }
                if(FatVolume_ReadSector(&self->FatVolume,sector,next_sector_buffer))
                {
                    if (self->Debug)
                    {
                        FatVolume_DebugSector(next_sector_buffer);
                        PS2Driver_WaitForKeyPress("SMVolume Pause");
                    }
                    path->WalkIndex++; 
                    if (self->Debug)
                    {
                        printf("SMVolume: Walking to dir %d\n",path->WalkIndex);
                        PS2Driver_WaitForKeyPress("SMVolume Pause");
                    }
                    return SMVolume_GetDirectory(self, dir, next_sector_buffer, path);
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return false;
}