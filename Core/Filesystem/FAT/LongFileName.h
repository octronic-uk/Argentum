#pragma once

#include <stdint.h>
#include <stdbool.h>

#define FAT_LFN_NAME_1_SIZE  10
#define FAT_LFN_NAME_2_SIZE  12
#define FAT_LFN_NAME_3_SIZE  4
#define FAT_LFN_NAME_FULL_SIZE ( \
    FAT_LFN_NAME_1_SIZE+ \
    FAT_LFN_NAME_2_SIZE+ \
    FAT_LFN_NAME_3_SIZE+ \
    1 \
)

#define FAT_LFN_ATTR_IS_LFN  0x0F
#define FAT_LFN_ENTRY_NUMBER 0x0F
#define FAT_LFN_LAST_ENTRY   0x40


typedef struct
{
    uint8_t Index;
    uint8_t Characters1[FAT_LFN_NAME_1_SIZE];
    uint8_t Attributes;
    uint8_t EntryType;
    uint8_t Checksum;
    uint8_t Characters2[FAT_LFN_NAME_2_SIZE];
    uint16_t Zero;
    uint8_t Characters3[FAT_LFN_NAME_3_SIZE];
} 
__attribute__((packed))
FatLongFileName;


void FatLongFileName_Debug(FatLongFileName* lfn);

bool FatLongFileName_IsLfn(FatLongFileName* lfn);
uint8_t FatLongFileName_GetEntryNumber(FatLongFileName* lfn);
bool FatLongFileName_IsLastEntry(FatLongFileName* lfn);
void FatLongFileName_GetFileName(FatLongFileName* lfn, uint8_t* buffer);