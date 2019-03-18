#pragma once

#include "../../../Boot/multiboot.h"
#include <LibC/include/stdint.h>

#define UPPER_RAM_BASE   0x0100000
#define UPPER_RAM_OFFSET 0x0010000

typedef struct
{
	uint32_t mSize;
	uint8_t mInUse;
} Memory_BlockHeader;

typedef struct
{
	uint32_t mBlocksUsed;
	uint32_t mSizeInBytes;
} Memory_BlockCount;

static multiboot_info_t* Memory_MultibootInfo;
static uint32_t Memory_UpperRamSize;
static uint32_t Memory_BaseAddress;
static uint32_t Memory_HeapEndAddress;
static uint32_t Memory_BlocksBegin;

void Memory_Constructor(multiboot_info_t* mbi);
void Memory_Destructor();

void Memory_Detect();
uint32_t Memory_FindFreeBlock( uint32_t size);
void* Memory_Allocate( uint32_t size);
void Memory_Free( void* block);
void Memory_SetMultibootInfo( multiboot_info_t* mbi);
Memory_BlockCount Memory_CountUsedBlocks();
uint32_t Memory_MoveHeapEnd(int32_t delta);
uint32_t Memory_GetLastBlock();
Memory_BlockHeader* Memory_GetPreviousBlock( Memory_BlockHeader* block);


