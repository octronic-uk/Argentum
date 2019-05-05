#include "RamDisk.h"

#include <Objects/Kernel/Kernel.h>
#include <string.h>
#include <stdio.h>
#include "RD_FatHeader.h"

extern struct Kernel _Kernel;
static uint32_t index_seed = 0;

bool RamDisk_Constructor(struct RamDisk* self, uint32_t size_b)
{
    printf("RamDisk: Constructing\n");
    memset(self,0,sizeof(struct RamDisk));
    self->Exists = true;
    self->Debug = true;
    self->Size = size_b;
    self->Block = MemoryDriver_Allocate(&_Kernel.Memory, sizeof(uint8_t)*self->Size);
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

void RamDisk_Destructor(struct RamDisk* self)
{
    printf("RamDisk: Destructing disk %d\n",self->Index);
    MemoryDriver_Free(&_Kernel.Memory, self->Block);
}

uint8_t* RamDisk_ReadSectorLBA(struct RamDisk* self, uint32_t lba)
{
    uint32_t idx = lba*512;
    if (idx > self->Size)
    {
        return 0;
    }
    return &self->Block[idx];
}

bool RamDisk_WriteSectorLBA(struct RamDisk* self, uint32_t lba, uint8_t* data)
{
    uint32_t idx = lba*512;
    if (idx > self->Size)
    {
        return false;
    }
    memcpy(&self->Block[idx],data,512);
    return true;
}
