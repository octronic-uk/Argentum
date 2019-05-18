#pragma once

#include <Boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_UPPER_RAM_BASE   0x0100000
#define MEMORY_STACK_ALIGN 0x10
#define MEMORY_PHYSICAL_AREA_TABLE_SIZE 16

struct MemoryPhysicalArea
{
	uint32_t Address;
	uint32_t Length;
	uint32_t Type;
};
typedef struct MemoryPhysicalArea MemoryPhysicalArea;

struct MemoryBlockHeader
{
	uint32_t Size;
	uint8_t InUse;
	struct MemoryBlockHeader* Next;
};
typedef struct MemoryBlockHeader MemoryBlockHeader;

typedef struct MemoryBlockHeader MemoryBlockHeader;

struct MemoryDriver
{
	uint8_t Debug;
	multiboot_info_t* MultibootInfo;
	uint32_t HeapBaseAddress;
	uint32_t HeapSize;
	MemoryBlockHeader* StartBlock;
	MemoryPhysicalArea PhysicalAreas[MEMORY_PHYSICAL_AREA_TABLE_SIZE];
};

typedef struct MemoryDriver MemoryDriver;

bool MemoryDriver_Constructor(MemoryDriver* self);
void MemoryDriver_Destructor(MemoryDriver* self);

void* MemoryDriver_Allocate(MemoryDriver* self, uint32_t size);
void* MemoryDriver_Reallocate(MemoryDriver* self, void *ptr, uint32_t new_size);
void MemoryDriver_Free(MemoryDriver* self, void* block);

void MemoryDriver_SetMultibootInfo(MemoryDriver* self, multiboot_info_t* mbi);

// Internal

void MemoryDriver_Detect(MemoryDriver* self);
MemoryBlockHeader* MemoryDriver_ClaimFreeBlock(MemoryDriver* self, uint32_t size);
MemoryBlockHeader* MemoryDriver_GetHeaderFromValuePointer(MemoryDriver* self, void* value);
uint32_t MemoryDriver_CheckForUnusedNeighbors(MemoryDriver* self, MemoryBlockHeader* header,uint32_t requested_size);
MemoryBlockHeader* MemoryDriver_InsertDummyBlock(MemoryDriver* self, MemoryBlockHeader* resized_header);
void MemoryDriver_CleanUpHeap(MemoryDriver* self);
bool MemoryDriver_IsValidBlock(MemoryDriver* self, MemoryBlockHeader* header);

// Debug
void MemoryDriver_PrintMemoryMap(MemoryDriver* self);