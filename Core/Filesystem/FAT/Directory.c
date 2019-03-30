#include "Directory.h"
#include <stdio.h>
#include <string.h>

void FatDirectory_Debug(FatDirectory* fd)
{
    printf("FatDirectory: Structure\n");
    char name[9] = {0};
    char ext[4] = {0};
    memcpy(name,fd->Name,8);
    memcpy(ext,fd->Extension,3);

    printf("\tName:                    <%s>.<%s>\n",  name, ext);
    FatDirectory_DebugAttributes(fd->Attributes);
    printf("\tReserved 1:              0x%x\n", fd->Reserved1);
    printf("\tCreation Time Seconds:   0x%x\n", fd->CreationTimeMilliSeconds);
    printf("\tCreation Time:           0x%x\n", fd->CreationTime);
    printf("\tCreation Date:           0x%x\n", fd->CreationDate);
    printf("\tLast Accessed Date:      0x%x\n", fd->LastAccessedDate);
    printf("\tReserved 2:              0x%x\n", fd->Reserved2);
    printf("\tLast Modified Time:      0x%x\n", fd->LastModificationTime);
    printf("\tLast Modified Date:      0x%x\n", fd->LastModificationDate);
    printf("\tFirst Cluster Number:    0x%x\n", fd->FirstClusterNumber);
    printf("\tFile Size:               0x%x\n", fd->FileSize);
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

uint8_t FatDirectory_HasAttribute(FatDirectory* dir, uint8_t attr)
{
    return (dir->Attributes & attr ) != 0;
}
