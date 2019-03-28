#pragma once

#include "../../Boot/multiboot.h"
#include <stdint.h>

#define MEMORY_UPPER_RAM_BASE   0x0100000
#define MEMORY_UPPER_RAM_OFFSET 0x0040000
#define MEMORY_PHYSICAL_AREA_TABLE_SIZE 32

#define MEMORY_PAGE_SIZE 		4096 
#define MEMORY_PAGE_TABLE_SIZE  1024 
#define MEMORY_PAGE_NUMBER_MASK 0xFF000000
#define MEMORY_PAGE_OFFSET_MASK 0x00FF0000

typedef struct
{
	uint32_t Address;
	uint32_t Length;
	uint32_t Type;
} Memory_PhysicalMemoryArea;

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

typedef struct 
{
	uint32_t Page;
	uint32_t Frame;
} Memory_PageTableEntry;

typedef struct 
{
	uint32_t Page;
	uint32_t Offset;
} Memory_Page;

typedef struct 
{
	uint32_t Frame;
	uint32_t Offset;
} Memory_Frame;

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



void* Memory_GetPhysicalAddress(void* virtualAddress);
void  Memory_MapPage(void * physicalAddress, void * virtualAddress, unsigned int flags);