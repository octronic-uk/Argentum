#include "FatDirectoryEntryData.h"

#include <stdio.h>
#include <string.h>
#include <Drivers/PS2/PS2Driver.h>
#include "FatConstants.h"

void FatDirectoryEntryData_Debug(FatDirectoryEntryData* self)
{
    printf("FatDirectoryEntryData: Structure\n");
    char name[9] = {0};
    char ext[4] = {0};
    memcpy(name,self->Name,8);
    memcpy(ext,self->Extension,3);

    printf("\tName:                    <%s>.<%s>\n",  name, ext);
    FatDirectoryEntryData_DebugAttributes(self);
    printf("\tReserved 1:              0x%x\n", self->Reserved1);
    printf("\tCreation Time Seconds:   0x%x\n", self->CreationTimeMilliSeconds);
    printf("\tCreation Time:           0x%x\n", self->CreationTime);
    printf("\tCreation Date:           0x%x\n", self->CreationDate);
    printf("\tLast Accessed Date:      0x%x\n", self->LastAccessedDate);
    printf("\tReserved 2:              0x%x\n", self->Reserved2);
    printf("\tLast Modified Time:      0x%x\n", self->LastModificationTime);
    printf("\tLast Modified Date:      0x%x\n", self->LastModificationDate);
    printf("\tFirst Cluster Number:    0x%x\n", self->FirstClusterNumber);
    printf("\tFile Size:               0x%x\n", self->FileSize);

}

void FatDirectoryEntryData_DebugAttributes(FatDirectoryEntryData* self)
{
    printf("\tAttributes:              0x%x\n", self->Attributes);

    if ((self->Attributes & FAT_DIR_ATTR_LFN ) == FAT_DIR_ATTR_LFN)
    {
        printf("\t\tLong File Name\n");
    }
    else
    {
        if ((self->Attributes & FAT_DIR_ATTR_READ_ONLY) != 0)
        {
            printf("\t\tRead Only\n");
        }
        if ((self->Attributes & FAT_DIR_ATTR_HIDDEN) != 0)
        {
            printf("\t\tHidden\n");
        }
        if ((self->Attributes & FAT_DIR_ATTR_SYSTEM) != 0)
        {
            printf("\t\tSystem\n");
        }
        if ((self->Attributes & FAT_DIR_ATTR_VOLUME_ID) != 0)
        {
            printf("\t\tVolume ID\n");
        }
        if ((self->Attributes & FAT_DIR_ATTR_DIRECTORY) != 0)
        {
            printf("\t\tDirectory\n");
        }
        if ((self->Attributes & FAT_DIR_ATTR_ARCHIVE) != 0)
        {
            printf("\t\tArchive\n");
        }
    }
}

bool FatDirectoryEntryData_HasAttribute(FatDirectoryEntryData* self, uint8_t attr)
{
    return (self->Attributes == attr);
}

char* FatDirectoryEntryData_GetDirectoryTypeString(uint8_t attributes)
{
    switch (attributes)
    {
        case FAT_DIR_ATTR_DIRECTORY:
            return "DIR";
        case FAT_DIR_ATTR_ARCHIVE:
            return "ARC";
        case FAT_DIR_ATTR_READ_ONLY:
            return "R/O";
        case FAT_DIR_ATTR_HIDDEN:
            return "HDN";
        case FAT_DIR_ATTR_SYSTEM:
            return "SYS";
        case FAT_DIR_ATTR_VOLUME_ID:
            return "VOL";
        case FAT_DIR_ATTR_LFN:
            return "LFN";
        default:
            return "???";
    }
}