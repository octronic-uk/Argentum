#include "RamDisk.h"

#include <Objects/Kernel/Kernel.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "RD_FatHeader.h"

extern Kernel _Kernel;
static uint32_t index_seed = 0;

bool RamDisk_Constructor(RamDisk* self, uint32_t size_b)
{
    printf("RamDisk: Constructing\n");
    memset(self,0,sizeof(RamDisk));
    self->Exists = true;
    self->Debug = true;
    self->Size = size_b;
    self->Block = malloc(sizeof(uint8_t)*self->Size);
    memset(self->Block,0,sizeof(uint8_t)*self->Size);
    self->Index = index_seed++;
    if (self->Debug)
    {
        printf("RamDisk: Index=%d\tSize=%d bytes\tBlock=0x%x\n",self->Index,self->Size,(uint32_t)self->Block);
    }
    // Populate FAT16 Headers
    memcpy(self->Block,         rd_fat_header_0,     FH_0_COUNT    );
    memcpy(&self->Block[510],   rd_fat_header_510,   FH_510_COUNT  );
    memcpy(&self->Block[2048],  rd_fat_header_2048,  FH_2048_COUNT );
    memcpy(&self->Block[12288], rd_fat_header_12288, FH_12288_COUNT);
}

void RamDisk_Destructor(RamDisk* self)
{
    printf("RamDisk: Destructing disk %d\n",self->Index);
    free(self->Block);
}

bool RamDisk_ReadSectorLBA(RamDisk* self, uint32_t lba, uint8_t* buffer)
{
    uint32_t idx = lba*512;
    if (idx > self->Size)
    {
        return 0;
    }

    memcpy(buffer, &self->Block[idx], FAT_SECTOR_SIZE);
    return true;
}

bool RamDisk_WriteSectorLBA(RamDisk* self, uint32_t lba, uint8_t* data)
{
    uint32_t idx = lba*512;
    if (idx > self->Size)
    {
        return false;
    }
    memcpy(&self->Block[idx], data, FAT_SECTOR_SIZE);
    return true;
}
