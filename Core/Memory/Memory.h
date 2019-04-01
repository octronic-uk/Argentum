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

struct Memory_PhysicalMemoryArea
{
	uint32_t Address;
	uint32_t Length;
	uint32_t Type;
};

struct Memory_BlockHeader
{
	uint32_t mSize;
	uint8_t mInUse;
} ;

struct Memory_BlockCount
{
	uint32_t mBlocksUsed;
	uint32_t mSizeInBytes;
};

struct Memory_PageTableEntry
{
	uint32_t Page;
	uint32_t Frame;
};

struct Memory_Page
{
	uint32_t Page;
	uint32_t Offset;
};

struct Memory_Frame
{
	uint32_t Frame;
	uint32_t Offset;
};

struct Memory
{
	uint8_t Debug;
	multiboot_info_t* MultibootInfo;
	uint32_t UpperRamSize;
	uint32_t BaseAddress;
	uint32_t HeapEndAddress;
	uint32_t BlocksBegin;
	struct Memory_PageTableEntry PageTable[MEMORY_PAGE_TABLE_SIZE];
	struct Memory_PhysicalMemoryArea PhysicalAreas[MEMORY_PHYSICAL_AREA_TABLE_SIZE];
};

void Memory_Constructor(struct Memory* self, multiboot_info_t* mbi_header);
void Memory_Destructor(struct Memory* self);

void Memory_Detect(struct Memory* self);
uint32_t Memory_FindFreeBlock(struct Memory* self, uint32_t size);
void* Memory_Allocate(struct Memory* self, uint32_t size);
void Memory_Free(struct Memory* self, void* block);
void Memory_SetMultibootInfo(struct Memory* self, multiboot_info_t* mbi);
struct Memory_BlockCount Memory_CountUsedBlocks(struct Memory* self);
uint32_t Memory_MoveHeapEnd(struct Memory* self, int32_t delta);
uint32_t Memory_GetLastBlock(struct Memory* self);
struct Memory_BlockHeader* Memory_GetPreviousBlock(struct Memory* self,  struct Memory_BlockHeader* block);

void* Memory_GetPhysicalAddress(struct Memory* self, void* virtualAddress);
void  Memory_MapPage(struct Memory* self, void * physicalAddress, void * virtualAddress, unsigned int flags);