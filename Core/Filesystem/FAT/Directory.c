#include "Directory.h"
#include <stdio.h>
#include <string.h>

void FatDirectory_Debug(struct FatDirectory* self)
{
    printf("FatDirectory: Structure\n");
    char name[9] = {0};
    char ext[4] = {0};
    memcpy(name,self->Name,8);
    memcpy(ext,self->Extension,3);

    printf("\tName:                    <%s>.<%s>\n",  name, ext);
    FatDirectory_DebugAttributes(self->Attributes);
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

void FatDirectory_DebugAttributes(uint8_t attributes)
{
    printf("\tAttributes:              0x%x\n", attributes);

    if ((attributes & FAT_DIR_ATTR_LFN) == FAT_DIR_ATTR_LFN)
    {
        printf("\t\tLong File Name\n");
    }
    else
    {
        if ((attributes & FAT_DIR_ATTR_READ_ONLY) != 0)
        {
            printf("\t\tRead Only\n");
        }
        if ((attributes & FAT_DIR_ATTR_HIDDEN) != 0)
        {
            printf("\t\tHidden\n");
        }
        if ((attributes & FAT_DIR_ATTR_SYSTEM) != 0)
        {
            printf("\t\tSystem\n");
        }
        if ((attributes & FAT_DIR_ATTR_VOLUME_ID) != 0)
        {
            printf("\t\tVolume ID\n");
        }
        if ((attributes & FAT_DIR_ATTR_DIRECTORY) != 0)
        {
            printf("\t\tDirectory\n");
        }
        if ((attributes & FAT_DIR_ATTR_ARCHIVE) != 0)
        {
            printf("\t\tArchive\n");
        }
    }
}

uint8_t FatDirectory_HasAttribute(struct FatDirectory* dir, uint8_t attr)
{
    return (dir->Attributes & attr ) != 0;
}
