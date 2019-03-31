#include <stdio.h>
#include <string.h>
#include <Filesystem/FAT/LongFileName.h>

void FatLongFileName_Debug(FatLongFileName* lfn)
{
    uint8_t chars1[11], chars2[13], chars3[5];
    uint8_t full_name[FAT_LFN_NAME_FULL_SIZE];

    memset(chars1,0,11);
    memcpy(chars1, lfn->Characters1, 10);

    memset(chars2,0,13);
    memcpy(chars2, lfn->Characters2, 12);

    memset(chars3,0,5);
    memcpy(chars3, lfn->Characters3, 4);

    FatLongFileName_GetFileName(lfn, full_name);

    int i;

    printf(
        "FatLongFileName: Entry number %d, Last: %s\n",
        FatLongFileName_GetEntryNumber(lfn),
        FatLongFileName_IsLastEntry(lfn) == true ? "Yes" : "No" 
    );
    printf("\tIndex:       0x%x\n", lfn->Index);

    printf("\tCharacters1: ");
    for (i=0;i<FAT_LFN_NAME_1_SIZE; i++) printf("%c",lfn->Characters1[i] ? lfn->Characters1[i] : ' ');
    printf("\n");

    printf("\tAttributes:  0x%x\n", lfn->Attributes);
    printf("\tEntryType:   0x%x\n", lfn->EntryType);
    printf("\tChecksum:    0x%x\n", lfn->Checksum);

    printf("\tCharacters2: ");
    for (i=0;i<FAT_LFN_NAME_2_SIZE; i++) printf("%c",lfn->Characters2[i] ? lfn->Characters2[i] : ' ');
    printf("\n");

    printf("\tZero:        0x%x\n", lfn->Zero);
    printf("\tCharacters3: ");
    for (i=0;i<FAT_LFN_NAME_3_SIZE; i++) printf("%c",lfn->Characters3[i] ? lfn->Characters3[i] : ' ');
    printf("\n");

    printf("\tFull Name:   %s\n",   full_name);
}

uint8_t FatLongFileName_GetEntryNumber(FatLongFileName* lfn)
{
    return lfn->Index & FAT_LFN_ENTRY_NUMBER;
}

bool FatLongFileName_IsLastEntry(FatLongFileName* lfn)
{
    return (lfn->Index & FAT_LFN_LAST_ENTRY) == FAT_LFN_LAST_ENTRY;
}

bool FatLongFileName_IsLfn(FatLongFileName* lfn)
{
    return (lfn->Attributes & FAT_LFN_ATTR_IS_LFN) == FAT_LFN_ATTR_IS_LFN;
}

void FatLongFileName_GetFileName(FatLongFileName* lfn, uint8_t* buffer)
{
   memset(buffer,0,FAT_LFN_NAME_FULL_SIZE);
   int i, j;
   for (i=0, j=0; i<FAT_LFN_NAME_FULL_SIZE; i+=2, j++) 
   {
        uint8_t next = 0;
        if (i < FAT_LFN_NAME_1_SIZE)
        {
            next = lfn->Characters1[i];
        }
        else if (i < FAT_LFN_NAME_1_SIZE + FAT_LFN_NAME_2_SIZE)
        {
            next = lfn->Characters2[i-FAT_LFN_NAME_1_SIZE];
        }
        else if (i < FAT_LFN_NAME_1_SIZE + FAT_LFN_NAME_2_SIZE + FAT_LFN_NAME_3_SIZE)
        {
           next = lfn->Characters3[i-(FAT_LFN_NAME_1_SIZE+FAT_LFN_NAME_2_SIZE)];
        }

        if (!next)
        {
           break;
        }
       buffer[j] = next;
   }
}