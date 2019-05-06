#pragma once
#include <stdint.h>
#include <stdbool.h>

#define RAMDISK_SIZE_1MB 1024*1024

struct RamDisk
{
    bool     Debug;
    bool     Exists;
    uint32_t Index;
    uint32_t Size;
    char*    Block;
};

bool RamDisk_Constructor(struct RamDisk* self, uint32_t size_b);
void RamDisk_Destructor(struct RamDisk* self);
bool RamDisk_ReadSectorLBA(struct RamDisk* self, uint32_t lba, uint8_t* buffer);
bool RamDisk_WriteSectorLBA(struct RamDisk* self, uint32_t lba, uint8_t* data);