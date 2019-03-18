#include "Memory.h"
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

//#define __DEBUG_MEMORY

void Memory_Constructor(multiboot_info_t* mbi_header)
{
	Memory_MultibootInfo = mbi_header;
	Memory_BlocksBegin = 0;
	Memory_BaseAddress = 0;
	Memory_HeapEndAddress = 0;
	Memory_Detect();
}

uint32_t Memory_FindFreeBlock(uint32_t size)
{
	#ifdef __DEBUG_MEMORY
		printf("Memory: Finding a free block\n");
	#endif

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
			#ifdef __DEBUG_MEMORY
				printf("Memory: Current block in use 0x%x\n", current);
			#endif

			if ((uint32_t)current + sizeof(Memory_BlockHeader) + current->mSize != Memory_HeapEndAddress)
			{
				current = (Memory_BlockHeader*)(((uint32_t)current) + sizeof(Memory_BlockHeader) + current->mSize);

				#ifdef __DEBUG_MEMORY
					printf("Memory: Stepped to next 0x%s\n", current);
				#endif
			}
			// No next block, assign to end of heap
			else
			{
				current = (Memory_BlockHeader*)Memory_HeapEndAddress;
				#ifdef __DEBUG_MEMORY
					printf("Memory: No free block found, using heap end 0x%x\n" , current);
				#endif
				return (uint32_t)current;
			}
		}
		else if (current->mSize >= size)
		{
			#ifdef __DEBUG_MEMORY
				printf("Memory: Found unused block with enough space 0x%x\n" , current);
			#endif
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
		#ifdef __DEBUG_MEMORY
			printf("Memory: Cannot allocate 0 size block");
		#endif
		return 0;
	}

	uint32_t freeBlock = Memory_FindFreeBlock(size);
	uint32_t totalRequestedSize = sizeof(Memory_BlockHeader) + size;

	Memory_BlockHeader* blockHeader = ((Memory_BlockHeader*)freeBlock);
	Memory_BlockHeader* afterBlockHeader = blockHeader+1;

	#ifdef __DEBUG_MEMORY
		printf("Memory: Allocated Block HDR.0x%x DATA.0x%x END.0x%x\n",
               freeBlock, afterBlockHeader, freeBlock+totalRequestedSize);
	#endif

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
	#ifdef __DEBUG_MEMORY
		printf("Memory: Moving Heap End by %d bytes from 0x%x",delta, Memory_HeapEndAddress);
	#endif

	Memory_HeapEndAddress += delta;

	#ifdef __DEBUG_MEMORY
		printf("to 0x%x \n" , Memory_HeapEndAddress);
	#endif

	return Memory_HeapEndAddress;
}

void Memory_Free(void* addr)
{
	uint32_t block = (uint32_t)addr;
	Memory_BlockHeader* header = ((Memory_BlockHeader*)block)-1;
	#ifdef __DEBUG_MEMORY
		printf("Memory: Freeing Memory Block at 0x%x", header);
	#endif
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
		#ifdef __DEBUG_MEMORY
			printf("Memory: Multiboot Info Not Found!\n");
		#endif
		return;
	}

	struct multiboot_mmap_entry* mmap = 0;
	if (Memory_MultibootInfo->flags & MULTIBOOT_INFO_MEMORY)
	{
		uint32_t mem_lower = (uint32_t)Memory_MultibootInfo->mem_lower;
		uint32_t mem_upper = (uint32_t)Memory_MultibootInfo->mem_upper;
		#ifdef __DEBUG_MEMORY
            printf("Memory: Upper %d KB Lower %d KB\n",mem_lower,mem_upper);
		#endif
	}

	if (Memory_MultibootInfo->flags & MULTIBOOT_INFO_MEM_MAP)
	{
		#ifdef __DEBUG_MEMORY
		printf("MMap Addr: 0x%x, MMap Length: 0x%x \n",
               MemoryMultibootInfo->mmap_addr,MemoryMultibootInfo->mmap_length);
		#endif

		for
		(
			mmap = (struct multiboot_mmap_entry*)Memory_MultibootInfo->mmap_addr;
			(uint32_t)mmap < (Memory_MultibootInfo->mmap_addr + Memory_MultibootInfo->mmap_length);
			mmap = (struct multiboot_mmap_entry*)((uint32_t)mmap + mmap->size + sizeof(mmap->size))
		)
		{
			#ifdef __DEBUG_MEMORY

				uint32_t baseHigh = mmap->addr >> 32;
				uint32_t baseLow = mmap->addr & 0xFFFFFFFF;
				uint32_t lengthHigh = mmap->len >> 32;
				uint32_t lengthLow = mmap->len & 0xFFFFFFFF;
				uint32_t type = mmap->type;

				printf("Memory: Base H.0x%x L.0x%x | Length H.0x%x L.0x%x Type %d",
                       baseHigh, baseLow, lengthHigh, lengthLow, type);

			#endif

			if ((mmap->addr & 0xFFFFFFFF) == UPPER_RAM_BASE)
			{
				#ifdef __DEBUG_MEMORY
				printf(" <-- HEAP");
				#endif
				Memory_BaseAddress = UPPER_RAM_BASE+UPPER_RAM_OFFSET;
				Memory_UpperRamSize = mmap->len;
			}
			#ifdef __DEBUG_MEMORY
            	printf("\n");
			#endif
		}
	}
	#ifdef __DEBUG_MEMORY
		printf("Memory: Heap starts at 0x%x \n",Memory_BaseAddress);
		printf("Memory: Memory_BlockHeader header is %d bytes\n",sizeof(Memory_BlockHeader));
	#endif
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
