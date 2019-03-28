#include "Memory.h"
#include <stdio.h>
#include <string.h>

multiboot_info_t* Memory_MultibootInfo;
uint8_t Memory_Debug;
uint32_t Memory_UpperRamSize;
uint32_t Memory_BaseAddress;
uint32_t Memory_HeapEndAddress;
uint32_t Memory_BlocksBegin;

Memory_PageTableEntry Memory_PageTable[MEMORY_PAGE_TABLE_SIZE];
Memory_PhysicalMemoryArea Memory_PhysicalAreas[MEMORY_PHYSICAL_AREA_TABLE_SIZE];

void Memory_Constructor(multiboot_info_t* mbi_header)
{
	memset(Memory_PhysicalAreas,0,sizeof(Memory_PhysicalMemoryArea)*MEMORY_PHYSICAL_AREA_TABLE_SIZE);
	Memory_MultibootInfo = mbi_header;
	Memory_Debug = 0;
	Memory_UpperRamSize = 0;
	Memory_BaseAddress = 0;
	Memory_HeapEndAddress = 0;
	Memory_BlocksBegin = 0;

	Memory_Detect();
}

uint32_t Memory_FindFreeBlock(uint32_t size)
{
	if (Memory_Debug) 
	{
		printf("Memory: Finding a free block\n");
	}

	if (Memory_BlocksBegin == 0)
	{
		Memory_BlocksBegin = Memory_BaseAddress;
		Memory_HeapEndAddress = Memory_BaseAddress;
		return Memory_BaseAddress;
	}

	Memory_BlockHeader* current = (Memory_BlockHeader*)Memory_BlocksBegin;
	while((uint32_t)current != Memory_HeapEndAddress)
	{
		if (current->mInUse)
		{
			if (Memory_Debug) 
			{
				printf("Memory: Current block in use 0x%x\n", current);
			}

			if ((uint32_t)current + sizeof(Memory_BlockHeader) + current->mSize != Memory_HeapEndAddress)
			{
				current = (Memory_BlockHeader*)(((uint32_t)current) + sizeof(Memory_BlockHeader) + current->mSize);

				if (Memory_Debug) 
				{
					printf("Memory: Stepped to next 0x%s\n", current);
				}
			}
			// No next block, assign to end of heap
			else
			{
				current = (Memory_BlockHeader*)Memory_HeapEndAddress;
				if (Memory_Debug) 
				{
					printf("Memory: No free block found, using heap end 0x%x\n" , current);
				}
				return (uint32_t)current;
			}
		}
		else if (current->mSize >= size)
		{
			if (Memory_Debug) 
			{
				printf("Memory: Found unused block with enough space 0x%x\n" , current);
			}
			return (uint32_t)current;
		}
	}

	return (uint32_t)current;
}

Memory_BlockHeader* Memory_GetPreviousBlock(Memory_BlockHeader* block)
{
	Memory_BlockHeader* b = (Memory_BlockHeader*)Memory_BlocksBegin;
	while(b + sizeof(Memory_BlockHeader) + b->mSize != block)
	{
		b = (Memory_BlockHeader*)b+sizeof(Memory_BlockHeader)+b->mSize;
		if ((uint32_t)b > Memory_HeapEndAddress)
		{
			return 0;
		}
	}
	return b;
}

void* Memory_Allocate(uint32_t size)
{
	// Cannot allocate zero size block
	if (size == 0)
	{
		if (Memory_Debug) 
		{
			printf("Memory: Cannot allocate 0 size block");
		}
		return 0;
	}

	uint32_t freeBlock = Memory_FindFreeBlock(size);
	uint32_t totalRequestedSize = sizeof(Memory_BlockHeader) + size;

	Memory_BlockHeader* blockHeader = ((Memory_BlockHeader*)freeBlock);
	Memory_BlockHeader* afterBlockHeader = blockHeader+1;

	if (Memory_Debug) 
	{
		printf("Memory: Allocated Block HDR.0x%x DATA.0x%x END.0x%x\n",
               freeBlock, afterBlockHeader, freeBlock+totalRequestedSize);
	}

	if (freeBlock == Memory_HeapEndAddress)
	{
		blockHeader->mInUse = 1;
		blockHeader->mSize = size;
		Memory_MoveHeapEnd(totalRequestedSize);
	}
	memset(afterBlockHeader,0,size);
	return (void*)afterBlockHeader;
}

uint32_t Memory_MoveHeapEnd(int32_t delta)
{
	if (Memory_Debug) 
	{
		printf("Memory: Moving Heap End by %d bytes from 0x%x",delta, Memory_HeapEndAddress);
	}

	Memory_HeapEndAddress += delta;

	if (Memory_Debug) 
	{
		printf("to 0x%x \n" , Memory_HeapEndAddress);
	}

	return Memory_HeapEndAddress;
}

void Memory_Free(void* addr)
{
	uint32_t block = (uint32_t)addr;
	Memory_BlockHeader* header = ((Memory_BlockHeader*)block)-1;
	if (Memory_Debug) 
	{
		printf("Memory: Freeing Memory Block at 0x%x", header);
	}
	header->mInUse = 0;
	memset(addr,0,header->mSize);
	// Last in heap
	if ((uint32_t)Memory_HeapEndAddress == (uint32_t)header+sizeof(Memory_BlockHeader)+header->mSize)
	{
		// Set the previous block to mark the end of allocated memory
		uint32_t totalSize = sizeof(Memory_BlockHeader)+header->mSize;
		Memory_MoveHeapEnd(-totalSize);
	}
}

void Memory_Detect()
{
	if (!Memory_MultibootInfo)
	{
		if (Memory_Debug) 
		{
			printf("Memory: Multiboot Info Not Found!\n");
		}
		return;
	}

	struct multiboot_mmap_entry* mmap = 0;
	if (Memory_MultibootInfo->flags & MULTIBOOT_INFO_MEMORY)
	{
		uint32_t mem_lower = (uint32_t)Memory_MultibootInfo->mem_lower;
		uint32_t mem_upper = (uint32_t)Memory_MultibootInfo->mem_upper;
		if (Memory_Debug) 
		{
            printf("Memory: Upper %d KB Lower %d KB\n",mem_lower,mem_upper);
		}
	}

	if (Memory_MultibootInfo->flags & MULTIBOOT_INFO_MEM_MAP)
	{
		if (Memory_Debug) 
		{
			printf("MMap Addr: 0x%x, MMap Length: 0x%x \n",
               Memory_MultibootInfo->mmap_addr,Memory_MultibootInfo->mmap_length);
		}

		int physicalAreaIndex = 0;

		for
		(
			mmap = (struct multiboot_mmap_entry*)Memory_MultibootInfo->mmap_addr;
			(uint32_t)mmap < (Memory_MultibootInfo->mmap_addr + Memory_MultibootInfo->mmap_length);
			mmap = (struct multiboot_mmap_entry*)((uint32_t)mmap + mmap->size + sizeof(mmap->size))
		)
		{
			if (physicalAreaIndex > MEMORY_PHYSICAL_AREA_TABLE_SIZE)
			{
				break;
			}

			Memory_PhysicalAreas[physicalAreaIndex].Address = (uint32_t)mmap->addr;
			Memory_PhysicalAreas[physicalAreaIndex].Length = (uint32_t)mmap->len;
			Memory_PhysicalAreas[physicalAreaIndex].Type = (uint32_t)mmap->type;

			printf("Memory: Area %d: 0x%x %dKB (%dMB) Type %d",
				physicalAreaIndex, 
				Memory_PhysicalAreas[physicalAreaIndex].Address,
				Memory_PhysicalAreas[physicalAreaIndex].Length/1024,
				Memory_PhysicalAreas[physicalAreaIndex].Length/(1024*1024),
				Memory_PhysicalAreas[physicalAreaIndex].Type
			);

			if (Memory_PhysicalAreas[physicalAreaIndex].Address == MEMORY_UPPER_RAM_BASE)
			{
				printf(" <-- HEAP");
				Memory_BaseAddress = MEMORY_UPPER_RAM_BASE + MEMORY_UPPER_RAM_OFFSET;
				Memory_UpperRamSize = Memory_PhysicalAreas[physicalAreaIndex].Length - MEMORY_UPPER_RAM_OFFSET;
			}
         	printf("\n");
			physicalAreaIndex++;
		}
	}
	if (Memory_Debug) 
	{
		printf("Memory: Heap starts at 0x%x \n",Memory_BaseAddress);
		printf("Memory: Memory_BlockHeader header is %d bytes\n",sizeof(Memory_BlockHeader));
	}
}

void Memory_SetMultibootInfo(multiboot_info_t* mbi)
{
	Memory_MultibootInfo = mbi;
}

Memory_BlockCount Memory_CountUsedBlocks()
{
	Memory_BlockCount c;
	c.mBlocksUsed = 0;
	c.mSizeInBytes = 0;
	Memory_BlockHeader* current = (Memory_BlockHeader*)Memory_BlocksBegin;
	do
	{
		if (current->mInUse)
		{
			c.mBlocksUsed++;
			c.mSizeInBytes += sizeof(Memory_BlockHeader)+current->mSize;
		}
		current = (Memory_BlockHeader*)current + sizeof(Memory_BlockHeader) + current->mSize;
	}
	while (current < (Memory_BlockHeader*)Memory_HeapEndAddress);
	return c;
}

uint32_t Memory_GetLastBlock()
{
	Memory_BlockHeader* last = Memory_GetPreviousBlock((Memory_BlockHeader*)Memory_HeapEndAddress);
	return (uint32_t)last;
}

void* Memory_GetPhysicalAddress(void* virtualAddress)
{
	return 0;
}

void Memory_MapPage(void* physicalAddress, void* virtualAddress, unsigned int flags)
{
}