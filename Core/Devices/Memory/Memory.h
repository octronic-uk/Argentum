#pragma once

#include "../../../Boot/multiboot.h"
#include <LibC/include/stdint.h>

#define UPPER_RAM_BASE   0x0100000
#define UPPER_RAM_OFFSET 0x0010000

typedef struct
{
	uint32_t mSize;
	uint8_t mInUse;
} MemoryBlockHeader;

typedef struct
{
	uint32_t mBlocksUsed;
	uint32_t mSizeInBytes;
} MemoryBlockCount;

static multiboot_info_t* MemoryMultibootInfo;
static uint32_t MemoryUpperRamSize;
static uint32_t MemoryBaseAddress;
static uint32_t MemoryHeapEndAddress;
static uint32_t MemoryBlocksBegin;

void Memory_Constructor(multiboot_info_t* mbi);
void Memory_Destructor();

void Memory_Detect();
uint32_t Memory_FindFreeBlock( uint32_t size);
void* Memory_Allocate( uint32_t size);
void Memory_Free( void* block);
void Memory_SetMultibootInfo( multiboot_info_t* mbi);
MemoryBlockCount Memory_CountUsedBlocks();
uint32_t Memory_MoveHeapEnd(int32_t delta);
uint32_t Memory_GetLastBlock();
MemoryBlockHeader* Memory_GetPreviousBlock( MemoryBlockHeader* block);


