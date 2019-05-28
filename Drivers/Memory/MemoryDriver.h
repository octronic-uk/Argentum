#pragma once

#include <Boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_UPPER_RAM_BASE   0x0100000
#define MEMORY_STACK_ALIGN 0x10
#define MEMORY_PHYSICAL_AREA_TABLE_SIZE 16
#define FILE_BUFFER_SZ 128

typedef struct MemoryPhysicalArea
{
	uint32_t Address;
	uint32_t Length;
	uint32_t Type;
}
MemoryPhysicalArea;

typedef struct MemoryBlockHeader
{
	uint32_t Size;
	uint8_t  InUse;
	uint32_t Line;
	char     File[FILE_BUFFER_SZ];
	struct MemoryBlockHeader* Next;
} 
MemoryBlockHeader;

typedef struct MemoryDriver
{
	uint8_t Debug;
	bool ShowSizeSet;
	multiboot_info_t* MultibootInfo;
	uint32_t HeapBaseAddress;
	uint32_t HeapSize;
	MemoryBlockHeader* StartBlock;
	MemoryPhysicalArea PhysicalAreas[MEMORY_PHYSICAL_AREA_TABLE_SIZE];
} MemoryDriver;

bool MemoryDriver_Constructor(MemoryDriver* self);
void MemoryDriver_Destructor(MemoryDriver* self);

void* MemoryDriver_Allocate(MemoryDriver* self, uint32_t size, char* file, uint32_t line);
void* MemoryDriver_Reallocate(MemoryDriver* self, void *ptr, uint32_t new_size, char* file, uint32_t line);
void MemoryDriver_Free(MemoryDriver* self, void* block, char* file, uint32_t line);

void MemoryDriver_SetMultibootInfo(MemoryDriver* self, multiboot_info_t* mbi);

// Internal

void MemoryDriver_Detect(MemoryDriver* self);
MemoryBlockHeader* MemoryDriver_ClaimFreeBlock(MemoryDriver* self, uint32_t size, char* file, uint32_t line);
MemoryBlockHeader* MemoryDriver_GetHeaderFromValuePointer(MemoryDriver* self, void* value, uint32_t line);
uint32_t MemoryDriver_CheckForUnusedNeighbors(MemoryDriver* self, MemoryBlockHeader* header,uint32_t requested_size);
MemoryBlockHeader* MemoryDriver_InsertDummyBlock(MemoryDriver* self, MemoryBlockHeader* resized_header, char* file, uint32_t line);
void MemoryDriver_CleanUpHeap(MemoryDriver* self);
bool MemoryDriver_IsValidBlock(MemoryDriver* self, MemoryBlockHeader* header);

// Debug
void MemoryDriver_PrintMemoryMap(MemoryDriver* self);