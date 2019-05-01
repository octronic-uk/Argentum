#pragma once

#include <Boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_UPPER_RAM_BASE   0x0100000
#define MEMORY_PHYSICAL_AREA_TABLE_SIZE 16

struct MemoryPhysicalArea
{
	uint32_t Address;
	uint32_t Length;
	uint32_t Type;
};

struct MemoryBlockHeader
{
	uint32_t Size;
	uint8_t InUse;
	struct MemoryBlockHeader* Next;
};

struct MemoryDriver
{
	uint8_t Debug;
	multiboot_info_t* MultibootInfo;
	uint32_t HeapBaseAddress;
	uint32_t HeapSize;
	struct MemoryBlockHeader* StartBlock;
	struct MemoryPhysicalArea PhysicalAreas[MEMORY_PHYSICAL_AREA_TABLE_SIZE];
};

bool MemoryDriver_Constructor(struct MemoryDriver* self);
void MemoryDriver_Destructor(struct MemoryDriver* self);

void* MemoryDriver_Allocate(struct MemoryDriver* self, uint32_t size);
void* MemoryDriver_Reallocate(struct MemoryDriver* self, void *ptr, uint32_t new_size);
void MemoryDriver_Free(struct MemoryDriver* self, void* block);

void MemoryDriver_SetMultibootInfo(struct MemoryDriver* self, multiboot_info_t* mbi);

// Internal

void MemoryDriver_Detect(struct MemoryDriver* self);
struct MemoryBlockHeader* MemoryDriver_FindFreeBlock(struct MemoryDriver* self, uint32_t size);
struct MemoryBlockHeader* MemoryDriver_GetHeaderFromValuePointer(void* value);
uint32_t MemoryDriver_CheckForUnusedNeighbors(struct MemoryDriver* self, struct MemoryBlockHeader* header);

// Tests

void MemoryDriver_TestAllocate(struct MemoryDriver* self);
void MemoryDriver_TestReallocate(struct MemoryDriver* self);
void MemoryDriver_TestFree(struct MemoryDriver* self);