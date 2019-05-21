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

typedef struct RamDisk RamDisk;

bool RamDisk_Constructor(RamDisk* self, uint32_t size_b);
void RamDisk_Destructor(RamDisk* self);
bool RamDisk_ReadSectorLBA(RamDisk* self, uint32_t lba, uint8_t* buffer);
bool RamDisk_WriteSectorLBA(RamDisk* self, uint32_t lba, uint8_t* data);