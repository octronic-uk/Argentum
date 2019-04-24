#include "SMVolume.h"

#include <Objects/Kernel/Kernel.h>
#include "../FAT16/FatVolume.h"
#include "SM_ATADrive.h"
#include "SMDirectoryEntry.h"
#include "SMFile.h"

#include <stdio.h>
#include <string.h>

extern struct Kernel _Kernel;

bool SMVolume_Constructor(struct SMVolume* self, struct SM_ATADrive* parent, uint8_t volume_index, uint32_t first_sector, uint32_t sectors_in_partition)
{
    memset(self,0,sizeof(struct SMVolume));
    self->Debug = true;
    self->ParentDrive = parent;
    self->VolumeIndex = volume_index;
    self->FirstSectorIndex = first_sector;
    self->SectorsInPartition = sectors_in_partition;

    if (FatVolume_Constructor(&self->FatVolume, self->ParentDrive->AtaIndex, self->VolumeIndex, self->FirstSectorIndex, self->SectorsInPartition))
    {
       return true;
    }
    printf("SMVolume: Error - Unable to construct corresponding FatVolume\n");
    return false;
}

bool SMVolume_GetDirectory(struct SMVolume* self, struct SMDirectoryEntry* out, uint8_t* sector_buffer, struct SMPath* path)
{
    struct FatTableListing fatListing;
    if (!FatTableListing_Constructor(&fatListing, &self->FatVolume, sector_buffer))
    {
        printf("SMVolume: Error - Could not read directory listing\n");
        return false;
    }

    if (self->Debug) FatTableListing_Debug(&fatListing);

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
                return SMDirectoryEntry_Constructor(out,self,entry->FirstCluster, entry->ClusterData.Attributes);
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
                    return SMVolume_GetDirectory(self, out, next_sector_buffer, path);
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