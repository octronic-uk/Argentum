#include "FatLfnCluster.h"

#include <stdio.h>
#include <string.h>

void FatLfnCluster_Debug(struct FatLfnCluster* self)
{
    uint8_t chars1[11], chars2[13], chars3[5];
    uint8_t full_name[FAT_LFN_NAME_FULL_SIZE];

    memset(chars1,0,11);
    memcpy(chars1, self->Characters1, 10);

    memset(chars2,0,13);
    memcpy(chars2, self->Characters2, 12);

    memset(chars3,0,5);
    memcpy(chars3, self->Characters3, 4);

    FatLfnCluster_GetFileName(self, full_name);

    int i;

    printf(
        "FatLFN: Entry number %d, Last: %s\n",
        FatLfnCluster_GetEntryNumber(self),
        FatLfnCluster_IsLastEntry(self) == true ? "Yes" : "No" 
    );
    printf("\tIndex:       0x%x\n", self->Index);

    printf("\tCharacters1: ");
    for (i=0;i<FAT_LFN_NAME_1_SIZE; i++) printf("%c",self->Characters1[i] ? self->Characters1[i] : ' ');
    printf("\n");

    printf("\tAttributes:  0x%x\n", self->Attributes);
    printf("\tEntryType:   0x%x\n", self->EntryType);
    printf("\tChecksum:    0x%x\n", self->Checksum);

    printf("\tCharacters2: ");
    for (i=0;i<FAT_LFN_NAME_2_SIZE; i++) printf("%c",self->Characters2[i] ? self->Characters2[i] : ' ');
    printf("\n");

    printf("\tZero:        0x%x\n", self->Zero);
    printf("\tCharacters3: ");
    for (i=0;i<FAT_LFN_NAME_3_SIZE; i++) printf("%c",self->Characters3[i] ? self->Characters3[i] : ' ');
    printf("\n");

    printf("\tFull Name:   %s\n",   full_name);
}

uint8_t FatLfnCluster_GetEntryNumber(struct FatLfnCluster* self)
{
    return self->Index & FAT_LFN_ENTRY_NUMBER;
}

bool FatLfnCluster_IsLastEntry(struct FatLfnCluster* self)
{
    return (self->Index & FAT_LFN_LAST_ENTRY) == FAT_LFN_LAST_ENTRY;
}

bool FatLfnCluster_IsLfn(struct FatLfnCluster* self)
{
    return self->Attributes == FAT_LFN_ATTR_IS_LFN;
}

void FatLfnCluster_GetFileName(struct FatLfnCluster* self, uint8_t* buffer)
{
   memset(buffer,0,FAT_LFN_NAME_FULL_SIZE);
   int i, j;
   for (i=0, j=0; i<FAT_LFN_NAME_FULL_SIZE; i+=2, j++) 
   {
        uint8_t next = 0;
        if (i < FAT_LFN_NAME_1_SIZE)
        {
            next = self->Characters1[i];
        }
        else if (i < FAT_LFN_NAME_1_SIZE + FAT_LFN_NAME_2_SIZE)
        {
            next = self->Characters2[i-FAT_LFN_NAME_1_SIZE];
        }
        else if (i < FAT_LFN_NAME_1_SIZE + FAT_LFN_NAME_2_SIZE + FAT_LFN_NAME_3_SIZE)
        {
           next = self->Characters3[i-(FAT_LFN_NAME_1_SIZE+FAT_LFN_NAME_2_SIZE)];
        }

        if (!next)
        {
           break;
        }
       buffer[j] = next;
   }
}