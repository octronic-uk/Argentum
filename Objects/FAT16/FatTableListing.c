#include "FatTableListing.h"

#include <string.h>
#include <stdio.h>
#include <Drivers/PS2/PS2Driver.h>
#include "FatVolume.h"
#include "FatLfnCluster.h"
#include "FatDirectoryEntryData.h"


bool FatTableListing_Constructor(struct FatTableListing* self, struct FatVolume* volume, uint8_t* cluster_data_start)
{
    // Init
    memset(self,0,sizeof(struct FatTableListing));
    self->Debug = false;
    self->Volume = volume;

    char lfn_name_current[FAT_LFN_NAME_SIZE];
    char lfn_name_tmp[FAT_LFN_NAME_SIZE];
    bool lfn_pending = 0;

    memset(lfn_name_current, 0, FAT_LFN_NAME_SIZE);
    memset(lfn_name_tmp, 0, FAT_LFN_NAME_SIZE);

    uint32_t entry_index = 0;
    uint8_t* cluster_data = cluster_data_start;

    // Don't read past the end of the sector buffer
    while(*cluster_data && cluster_data < (cluster_data_start + FAT_SECTOR_SIZE))    
    {
        if (*cluster_data != FAT_VOLUME_CLUSTER_UNUSED)
        {
            struct FatLfnCluster* lfn = (struct FatLfnCluster*)cluster_data;
            // Found an LFN Cluster
            if (FatLfnCluster_IsLfn(lfn))
            {
                lfn_pending = true;
                FatLfnCluster_GetFileName(lfn, lfn_name_tmp);
                strcat(lfn_name_tmp, lfn_name_current);
                memcpy(lfn_name_current, lfn_name_tmp, FAT_LFN_NAME_SIZE);
                memset(lfn_name_tmp, 0, FAT_LFN_NAME_SIZE);
            }
            // Not an LFN Cluster
            else 
            {
                // Previous Cluster WAS LFN
                if (lfn_pending)
                {
                    struct FatDirectoryEntryData* lfn_dir = (struct FatDirectoryEntryData*)cluster_data;
                    uint32_t lfn_first_cluster = lfn_dir->FirstClusterNumber;
                    uint32_t lfn_first_sector = FatVolume_GetFirstSectorOfCluster(self->Volume, lfn_first_cluster);
                    if (self->Debug) 
                    {
                        printf(
                            "FatTableListing: Found LFN: %s\n\tFirst Cluster at sector 0x%x (Phys 0x%x)\n",
                            lfn_name_current,
                            lfn_first_sector,
                            self->Volume->FirstSectorNumber + lfn_first_sector
                        );
                    }

                    FatDirectoryEntrySummary_Constructor(
                        &self->Entries[entry_index], 
                        self->Volume, 
                        lfn_name_current,
                        lfn_first_sector,
                        lfn_first_cluster,
                        lfn_dir->Attributes,
                        lfn_dir->FileSize
                    );

                    memset(lfn_name_tmp,     0, FAT_LFN_NAME_SIZE);
                    memset(lfn_name_current, 0, FAT_LFN_NAME_SIZE);
                    lfn_pending = false;
                    entry_index++;
                }
                // No Previous LFN Pending
                else
                {
                    struct FatDirectoryEntryData* dir = (struct FatDirectoryEntryData*)cluster_data;
                    char full_name[FAT_LFN_NAME_SIZE] = {' '};
                    if (FatDirectoryEntryData_HasAttribute(dir,FAT_DIR_ATTR_ARCHIVE))
                    {
                        full_name[8] = '.';
                    }
                    memcpy(full_name,dir->Name,8);
                    memcpy(full_name+9,dir->Extension,3);

                    uint32_t first_sector = FatVolume_GetFirstSectorOfCluster(self->Volume, dir->FirstClusterNumber);
                    if (self->Debug) 
                    {
                        printf(
                            "FatTableListing: \n"
                            "\tFound Non-LFN: %s\n"
                            "\tFirst Cluster at sector 0x%x (Phys 0x%x)\n",
                            full_name,
                            first_sector,
                            self->Volume->FirstSectorNumber + first_sector
                        );
                    }

                    FatDirectoryEntrySummary_Constructor(
                        &self->Entries[entry_index], 
                        self->Volume, 
                        full_name,
                        first_sector,
                        dir->FirstClusterNumber,
                        dir->Attributes,
                        dir->FileSize
                    );

                    entry_index++;
                }
            }
        }
        else
        {
            if (self->Debug) 
            {
                printf("FatTableListing: Unused Cluster\n");
            }
        }
        cluster_data += FAT_CLUSTER_SIZE_B;
    }

    self->EntryCount = entry_index;
    if (self->Debug) 
    {
        printf("FatTableListing: Reached end of cluster chain, found %d entries\n",entry_index);
    }
    return true;
}

void FatTableListing_Debug(struct FatTableListing* self)
{
    printf("FatTableListing: Debugging Listing\n");
    uint32_t count = self->EntryCount;
    uint32_t i;
    for (i=0; i<count; i++)
    {
        struct FatDirectoryEntrySummary *e = &self->Entries[i];
        char* type = FatDirectoryEntryData_GetDirectoryTypeString(e->Attributes);
        printf("\t-> (%s) %08dB %s \n", type,  e->FileSize, e->Name);
    }
}