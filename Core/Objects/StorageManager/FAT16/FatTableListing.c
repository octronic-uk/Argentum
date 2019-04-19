#include "FatTableListing.h"

#include <string.h>
#include <stdio.h>
#include <Drivers/PS2/PS2Driver.h>
#include "FatVolume.h"
#include "FatLfnCluster.h"
#include "FatDirectoryCluster.h"

/**
    @brief Parse the FAT Table.

    After the cluster has been loaded into memory, the next step is to read and parse all 
    of the entries in it. Each entry is 32 bytes long. For each 32 byte entry this is the flow of 
    execution:

    1 If the first byte of the entry is equal to 0 then there are no more files/directories in this 
      directory. 
        FirstByte == 0, finish. 
        FirstByte != 0, goto 2.

    2 If the first byte of the entry is equal to 0xE5 then the entry is unused. 
        FirstByte == 0xE5, goto 8, 
        FirstByte != 0xE5, goto 3.

    3 Is this entry a long file name entry? If the 11'th byte of the entry equals 0x0F, then it is a
      long file name entry.
        11thByte==0x0F, Goto 4. 
        11thByte!=0x0F, Goto 5.

    4 Read the portion of the long filename into a temporary buffer. 
        Goto 8.

    5 Parse the data for this entry using the FatDirectory table.
        Goto 6

    6 Is there a long file name in the temporary buffer? 
        Yes, Goto 7. 
        No,  Goto 8

    7 Apply the long file name to the entry that you just read and clear the temporary buffer. 
        Goto 8

    8 Increment pointers and/or counters and check the next entry. 
        Goto 1
*/
bool FatTableListing_Constructor(struct FatTableListing* self, struct FatVolume* volume, uint8_t* cluster_data)
{
    // Init
    memset(self,0,sizeof(struct FatTableListing));
    self->Debug = 0;
    self->Volume = volume;

    char lfn_name_current[FAT_LFN_NAME_SIZE];
    char lfn_name_tmp[FAT_LFN_NAME_SIZE];
    bool lfn_pending = 0;

    memset(lfn_name_current, 0, FAT_LFN_NAME_SIZE);
    memset(lfn_name_tmp, 0, FAT_LFN_NAME_SIZE);

    uint32_t entry_index = 0;

    while(*cluster_data)    
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
                    struct FatDirectoryCluster* lfn_dir = (struct FatDirectoryCluster*)cluster_data;
                    uint32_t lfn_first_cluster = lfn_dir->FirstClusterNumber;
                    uint32_t lfn_first_sector = FatVolume_GetFirstSectorOfCluster(self->Volume, lfn_first_cluster);
                    if (self->Debug) {
                        printf(
                            "FatTableListing: Found LFN: %s\n\tFirst Cluster at sector 0x%x (Phys 0x%x)\n",
                            lfn_name_current,
                            lfn_first_sector,
                            self->Volume->FirstSectorNumber + lfn_first_sector
                        );
                        PS2Driver_WaitForKeyPress("Fat Dir Listing");
                    }

                    FatDirectoryEntry_Constructor(
                        &self->Entries[entry_index], 
                        self->Volume, 
                        lfn_dir,
                        lfn_name_current,
                        lfn_first_sector,
                        lfn_first_cluster
                    );

                    memset(lfn_name_tmp,     0, FAT_LFN_NAME_SIZE);
                    memset(lfn_name_current, 0, FAT_LFN_NAME_SIZE);
                    lfn_pending = false;
                    entry_index++;
                }
                else
                {
                    struct FatDirectoryCluster* dir = (struct FatDirectoryCluster*)cluster_data;
                    char full_name[FAT_LFN_NAME_SIZE];
                    full_name[8] = '.';
                    memset(full_name,0,FAT_LFN_NAME_SIZE);

                    memcpy(full_name,dir->Name,8);
                    memcpy(full_name+9,dir->Extension,3);

                    uint32_t first_sector = FatVolume_GetFirstSectorOfCluster(self->Volume, dir->FirstClusterNumber);
                    if (self->Debug) 
                    {
                        printf(
                            "FatTableListing: Found Non-LFN: %s\n\tFirst Cluster at sector 0x%x (Phys 0x%x)\n",
                            full_name,
                            first_sector,
                            self->Volume->FirstSectorNumber + first_sector
                        );
                        PS2Driver_WaitForKeyPress("Fat Dir Listing");
                    }

                    FatDirectoryEntry_Constructor(
                        &self->Entries[entry_index], 
                        self->Volume, 
                        dir,
                        full_name,
                        first_sector,
                        dir->FirstClusterNumber
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
        struct FatDirectoryEntry *e = &self->Entries[i];
        char* type = FatDirectoryCluster_GetDirectoryTypeString(&e->ClusterData);
        printf("\t-> (%s) %08dB %s \n", type,  e->ClusterData.FileSize, e->Name);
    }
    PS2Driver_WaitForKeyPress("Fat Dir Listing");
}