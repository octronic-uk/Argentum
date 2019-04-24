#pragma once

#include <Boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>

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

struct MemoryDriver
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

bool MemoryDriver_Constructor(struct MemoryDriver* self);
void MemoryDriver_Destructor(struct MemoryDriver* self);

void MemoryDriver_Detect(struct MemoryDriver* self);
uint32_t MemoryDriver_FindFreeBlock(struct MemoryDriver* self, uint32_t size);
void* MemoryDriver_Allocate(struct MemoryDriver* self, uint32_t size);
void MemoryDriver_Free(struct MemoryDriver* self, void* block);
void MemoryDriver_SetMultibootInfo(struct MemoryDriver* self, multiboot_info_t* mbi);
struct Memory_BlockCount MemoryDriver_CountUsedBlocks(struct MemoryDriver* self);
uint32_t MemoryDriver_MoveHeapEnd(struct MemoryDriver* self, int32_t delta);
uint32_t MemoryDriver_GetLastBlock(struct MemoryDriver* self);
struct Memory_BlockHeader* MemoryDriver_GetPreviousBlock(struct MemoryDriver* self,  struct Memory_BlockHeader* block);

void* MemoryDriver_GetPhysicalAddress(struct MemoryDriver* self, void* virtualAddress);
void  MemoryDriver_MapPage(struct MemoryDriver* self, void * physicalAddress, void * virtualAddress, unsigned int flags);