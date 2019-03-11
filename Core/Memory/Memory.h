#pragma once
#include "../../Boot/multiboot.h"
#include "../Types.h"

#define UPPER_RAM_BASE   0x0100000
#define UPPER_RAM_OFFSET 0x0010000

struct tkMemory_BlockHeaderStruct
{
    uint32_t mSize;
    uint8_t mInUse;
} __attribute__((packed));

typedef struct tkMemory_BlockHeaderStruct tkMemory_BlockHeader;

typedef struct
{
    uint32_t mBlocksUsed;
    uint32_t mSizeInBytes;
} __attribute__((packed)) tkMemory_BlockCount;


static multiboot_info_t* MemoryMultibootInfo;

static uint64_t MemoryUpperRamSize;
static uint32_t MemoryBaseAddress;
static uint32_t MemoryHeapEndAddress;

static uint32_t MemoryBlocksBegin;

void tkMemory_Initialise();
void tkMemory_Detect();
uint32_t tkMemory_FindFreeBlock(uint32_t size);
void* tkMemory_Allocate(uint32_t size);
void tkMemory_Free(void* block);
uint32_t tkMemory_MoveHeapEnd(int32_t delta);
void tkMemory_SetMultibootInfo(multiboot_info_t* mbi);
tkMemory_BlockCount tkMemory_CountUsedBlocks();
uint32_t tkMemory_GetLastBlock();
tkMemory_BlockHeader* tkMemory_GetPreviousBlock(tkMemory_BlockHeader* block);