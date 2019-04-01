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


struct FatLongFileName
{
    uint8_t Index;
    uint8_t Characters1[FAT_LFN_NAME_1_SIZE];
    uint8_t Attributes;
    uint8_t EntryType;
    uint8_t Checksum;
    uint8_t Characters2[FAT_LFN_NAME_2_SIZE];
    uint16_t Zero;
    uint8_t Characters3[FAT_LFN_NAME_3_SIZE];
} __attribute__((packed));


void FatLongFileName_Debug(struct FatLongFileName* self);

bool FatLongFileName_IsLfn(struct FatLongFileName* self);
uint8_t FatLongFileName_GetEntryNumber(struct FatLongFileName* self);
bool FatLongFileName_IsLastEntry(struct FatLongFileName* self);
void FatLongFileName_GetFileName(struct FatLongFileName* self, uint8_t* buffer);