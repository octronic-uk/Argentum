#pragma once
#include "../../Boot/multiboot.h"
#include "../Types.h"

#define __DEBUG_MEMORY 1

#define UPPER_RAM_BASE   0x0100000
#define UPPER_RAM_OFFSET 0x0010000

struct tkMemoryBlockHeaderStruct
{
    uint32_t mSize;
    uint8_t mInUse;
} __attribute__((packed));

typedef struct tkMemoryBlockHeaderStruct tkMemoryBlockHeader;

typedef struct
{
    uint32_t mBlocksUsed;
    uint32_t mSizeInBytes;
} __attribute__((packed)) tkMemoryBlockCount;


static multiboot_info_t* MemoryMultibootInfo;

static uint64_t MemoryUpperRamSize;
static uint32_t MemoryBaseAddress;
static uint32_t MemoryHeapEndAddress;

static uint32_t MemoryBlocksBegin;

void tkMemoryInitialise();
void tkMemoryDetect();
uint32_t tkMemoryFindFreeBlock(uint32_t size);
void* tkMemoryAllocate(uint32_t size);
void tkMemoryFree(void* block);
uint32_t tkMemoryMoveHeapEnd(int32_t delta);
void tkMemorySetMultibootInfo(multiboot_info_t* mbi);
tkMemoryBlockCount tkMemoryCountUsedBlocks();
uint32_t tkMemoryGetLastBlock();
tkMemoryBlockHeader* tkMemoryGetPreviousBlock(tkMemoryBlockHeader* block);