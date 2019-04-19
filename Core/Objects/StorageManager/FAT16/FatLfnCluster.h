#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "FatConstants.h"

struct FatLfnCluster
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

void FatLfnCluster_Debug(struct FatLfnCluster* self);
bool FatLfnCluster_IsLfn(struct FatLfnCluster* self);
uint8_t FatLfnCluster_GetEntryNumber(struct FatLfnCluster* self);
bool FatLfnCluster_IsLastEntry(struct FatLfnCluster* self);
void FatLfnCluster_GetFileName(struct FatLfnCluster* self, uint8_t* buffer);