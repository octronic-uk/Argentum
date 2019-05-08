#include "SMVolume.h"

#include <stdio.h>
#include <string.h>

#include <Objects/Kernel/Kernel.h>
#include <Objects/FAT/FatVolume.h>
#include <Objects/StorageManager/SMDrive.h>
#include <Objects/StorageManager/SMDirectoryEntry.h>


extern struct Kernel _Kernel;

bool SMVolume_ATAConstructor(struct SMVolume* self, struct SMDrive* parent, uint8_t volume_index, uint32_t first_sector, uint32_t sectors_in_partition)
{
    memset(self,0,sizeof(struct SMVolume));
    self->Debug = false;
    self->Exists = true;
    self->ParentDrive = parent;
    self->VolumeIndex = volume_index;
    self->FirstSectorIndex = first_sector;
    self->SectorsInPartition = sectors_in_partition;

    if (FatVolume_ATAConstructor(&self->FatVolume, self->ParentDrive->AtaIndex, self->VolumeIndex, self->FirstSectorIndex, self->SectorsInPartition))
    {
        if (FatVolume_ReadFileAllocationTableData(&self->FatVolume))
        {
            if (self->Debug) SMVolume_DebugRootDirectorySize(self);
            return true;
        }
    }
    printf("SMVolume: Error - Unable to construct corresponding ATA FatVolume\n");
    return false;
}

bool SMVolume_FloppyConstructor(struct SMVolume* self, struct SMDrive* parent, uint32_t sectors_in_partition)
{
    memset(self,0,sizeof(struct SMVolume));

    self->Debug = false;
    self->Exists = true;
    self->ParentDrive = parent;
    self->SectorsInPartition = FLOPPY_144_NUM_SECTORS;

    if (FatVolume_FloppyConstructor(&self->FatVolume, parent->FloppyIndex))
    {
        if (FatVolume_ReadFileAllocationTableData(&self->FatVolume))
        {
            if (self->Debug) SMVolume_DebugRootDirectorySize(self);
            return true;
        }
    }

    printf("SMVolume: Error - Unable to construct corresponding Floppy FatVolume\n");
    return false;
}

bool SMVolume_RamDiskConstructor(struct SMVolume* self, struct SMDrive* parent, uint32_t sectors_in_partition)
{
    memset(self,0,sizeof(struct SMVolume));

    self->Debug = false;
    self->Exists = true;
    self->ParentDrive = parent;
    self->SectorsInPartition = sectors_in_partition;

    if (FatVolume_RamDiskConstructor(&self->FatVolume, parent->RamDiskIndex))
    {
        if (FatVolume_ReadFileAllocationTableData(&self->FatVolume))
        {
            if (self->Debug) SMVolume_DebugRootDirectorySize(self);
            return true;
        }
    }
    printf("SMVolume: Error - Unable to construct corresponding RamDisk FatVolume\n");
    return false;
}


void SMVolume_Destructor(struct SMVolume* self)
{
    FatVolume_Destructor(&self->FatVolume);
}
bool SMVolume_GetDirectory(
    struct SMVolume* self, struct SMDirectoryEntry* out, uint8_t* sector_buffer, 
    uint32_t sector_count, struct SMPath* path
){
    struct FatDirectoryListing fatListing;
    if (!FatDirectoryListing_Constructor(&fatListing, &self->FatVolume, sector_buffer, sector_count))
    {
        printf("SMVolume: Error - Could not read directory listing\n");
        FatDirectoryListing_Destructor(&fatListing);
        return false;
    }

    if (self->Debug) FatDirectoryListing_Debug(&fatListing);

    const char* target_name = LinkedList_At(&path->Directories,path->WalkIndex);
    uint8_t num_dirs = LinkedList_Size(&path->Directories);

    // Compare Path name to FAT entries
    uint8_t fat_entry_index;
    for (fat_entry_index = 0; fat_entry_index < fatListing.EntryCount; fat_entry_index++)
    {
        struct FatDirectoryEntrySummary* entry = LinkedList_At(&fatListing.Entries, fat_entry_index);
        char* entry_name = entry->Name;

        if (self->Debug)
        {
            printf("SMVolume: Comparing %s with target %s\n",entry_name, target_name);
        }

        // Chech each entry in the listing 
        if (strncmp(entry_name, target_name, FAT_LFN_NAME_SIZE) == 0)
        {
            printf("SMVolume: Found match %s at walk index %d/%d\n", target_name, path->WalkIndex, num_dirs);
            // Last directory in path
            if (path->WalkIndex == num_dirs-1)
            {
                if (self->Debug)
                {
                    printf("SMVolume: Last Directory in path is %s\n",target_name);
                }
                bool retval =  SMDirectoryEntry_Constructor(out,self,entry->FirstCluster, entry->Attributes);
                FatDirectoryListing_Destructor(&fatListing);
                return retval;
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
                }

                if(FatVolume_ReadSector(&self->FatVolume,sector,next_sector_buffer))
                {
                    if (self->Debug)
                    {
                        FatVolume_DebugSector(next_sector_buffer);
                    }
                    path->WalkIndex++; 
                    if (self->Debug)
                    {
                        printf("SMVolume: Walking to dir %d\n",path->WalkIndex);
                    }
                    bool retval = SMVolume_GetDirectory(self, out, next_sector_buffer, 1, path);
                    FatDirectoryListing_Destructor(&fatListing);
                    return retval;
                }
                else
                {
                    FatDirectoryListing_Destructor(&fatListing);
                    return false;
                }
            }
        }
    }
    FatDirectoryListing_Destructor(&fatListing);
    return false;
}

uint16_t SMVolume_GetRootDirectoryCount(struct SMVolume* self)
{
    return self->FatVolume.BiosParameterBlock.RootEntryCount;
}

uint32_t SMVolume_GetRootDirectorySectorCount(struct SMVolume* self)
{
    uint16_t num_root_directories = SMVolume_GetRootDirectoryCount(self);
    uint32_t root_dir_size_b = num_root_directories * 32 /* directory size */;
    return root_dir_size_b/self->FatVolume.BiosParameterBlock.BytesPerSector;
}

void SMVolume_DebugRootDirectorySize(struct SMVolume* self)
{
    uint16_t num_root_directories = SMVolume_GetRootDirectoryCount(self);
    uint32_t root_dir_size_b = num_root_directories * 32 /* directory size */;
    uint32_t root_dir_size_in_sectors = root_dir_size_b/self->FatVolume.BiosParameterBlock.BytesPerSector;
    printf("SMDVolume: The root dir has %d entries\n\tThat is is %d bytes, or %d sectors\n",
        num_root_directories,
        root_dir_size_b,
        root_dir_size_in_sectors
    );
}