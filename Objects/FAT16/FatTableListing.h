#pragma once

#include <stdbool.h>

#include "FatDirectoryEntryData.h"
#include "FatConstants.h"
#include "FatDirectoryEntrySummary.h"

struct FatVolume;

struct FatTableListing
{
    bool Debug;
    char Name[FAT_LFN_NAME_SIZE];
    struct FatVolume* Volume;
    uint32_t EntryCount;
    struct FatDirectoryEntrySummary Entries[FAT_DIR_LISTING_MAX_DIRS];
};

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
bool FatTableListing_Constructor(struct FatTableListing* self, struct FatVolume* volume, uint8_t* cluster_data);
void FatTableListing_Debug(struct FatTableListing* self);