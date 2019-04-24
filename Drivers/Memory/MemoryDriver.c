#include <Drivers/Memory/MemoryDriver.h>

#include <stdio.h>
#include <string.h>

#include <Objects/Kernel/Kernel.h>

extern struct Kernel _Kernel;

bool MemoryDriver_Constructor(struct MemoryDriver* self)
{
	memset(self->PhysicalAreas,0,sizeof(struct Memory_PhysicalMemoryArea)*MEMORY_PHYSICAL_AREA_TABLE_SIZE);
	self->MultibootInfo = _Kernel.MultibootInfo;
	self->Debug = 0;
	self->UpperRamSize = 0;
	self->BaseAddress = 0;
	self->HeapEndAddress = 0;
	self->BlocksBegin = 0;

	MemoryDriver_Detect(self);
	return true;
}

uint32_t MemoryDriver_FindFreeBlock(struct MemoryDriver* self, uint32_t size)
{
	if (self->Debug) 
	{
		printf("Memory: Finding a free block\n");
	}

	if (self->BlocksBegin == 0)
	{
		self->BlocksBegin = self->BaseAddress;
		self->HeapEndAddress = self->BaseAddress;
		return self->BaseAddress;
	}

	struct Memory_BlockHeader* current = (struct Memory_BlockHeader*)self->BlocksBegin;
	while((uint32_t)current != self->HeapEndAddress)
	{
		if (current->mInUse)
		{
			if (self->Debug) 
			{
				printf("Memory: Current block in use 0x%x\n", current);
			}

			if ((uint32_t)current + sizeof(struct Memory_BlockHeader) + current->mSize != self->HeapEndAddress)
			{
				current = (struct Memory_BlockHeader*)(((uint32_t)current) + sizeof(struct Memory_BlockHeader) + current->mSize);

				if (self->Debug) 
				{
					printf("Memory: Stepped to next 0x%s\n", current);
				}
			}
			// No next block, assign to end of heap
			else
			{
				current = (struct Memory_BlockHeader*)self->HeapEndAddress;
				if (self->Debug) 
				{
					printf("Memory: No free block found, using heap end 0x%x\n" , current);
				}
				return (uint32_t)current;
			}
		}
		else if (current->mSize >= size)
		{
			if (self->Debug) 
			{
				printf("Memory: Found unused block with enough space 0x%x\n" , current);
			}
			return (uint32_t)current;
		}
	}

	return (uint32_t)current;
}

struct Memory_BlockHeader* Memory_GetPreviousBlock(struct MemoryDriver* self, struct Memory_BlockHeader* block)
{
	struct Memory_BlockHeader* b = (struct Memory_BlockHeader*)self->BlocksBegin;
	while(b + sizeof(struct Memory_BlockHeader) + b->mSize != block)
	{
		b = (struct Memory_BlockHeader*)b+sizeof(struct Memory_BlockHeader)+b->mSize;
		if ((uint32_t)b > self->HeapEndAddress)
		{
			return 0;
		}
	}
	return b;
}

void* MemoryDriver_Allocate(struct MemoryDriver* self, uint32_t size)
{
	// Cannot allocate zero size block
	if (size == 0)
	{
		if (self->Debug) 
		{
			printf("Memory: Cannot allocate 0 size block");
		}
		return 0;
	}

	uint32_t freeBlock = MemoryDriver_FindFreeBlock(self, size);
	uint32_t totalRequestedSize = sizeof(struct Memory_BlockHeader) + size;

	struct Memory_BlockHeader* blockHeader = ((struct Memory_BlockHeader*)freeBlock);
	struct Memory_BlockHeader* afterBlockHeader = blockHeader+1;

	if (self->Debug) 
	{
		printf("Memory: Allocated Block HDR.0x%x DATA.0x%x END.0x%x\n",
               freeBlock, afterBlockHeader, freeBlock+totalRequestedSize);
	}

	if (freeBlock == self->HeapEndAddress)
	{
		blockHeader->mInUse = 1;
		blockHeader->mSize = size;
		MemoryDriver_MoveHeapEnd(self,totalRequestedSize);
	}
	memset(afterBlockHeader,0,size);
	return (void*)afterBlockHeader;
}

uint32_t MemoryDriver_MoveHeapEnd(struct MemoryDriver* self, int32_t delta)
{
	if (self->Debug) 
	{
		printf("Memory: Moving Heap End by %d bytes from 0x%x",delta, self->HeapEndAddress);
	}

	self->HeapEndAddress += delta;

	if (self->Debug) 
	{
		printf("to 0x%x \n" , self->HeapEndAddress);
	}

	return self->HeapEndAddress;
}

void MemoryDriver_Free(struct MemoryDriver* self, void* addr)
{
	uint32_t block = (uint32_t)addr;
	struct Memory_BlockHeader* header = ((struct Memory_BlockHeader*)block)-1;
	if (self->Debug) 
	{
		printf("Memory: Freeing Memory Block at 0x%x", header);
	}
	header->mInUse = 0;
	memset(addr,0,header->mSize);
	// Last in heap
	if ((uint32_t)self->HeapEndAddress == (uint32_t)header+sizeof(struct Memory_BlockHeader)+header->mSize)
	{
		// Set the previous block to mark the end of allocated memory
		uint32_t totalSize = sizeof(struct Memory_BlockHeader)+header->mSize;
		MemoryDriver_MoveHeapEnd(self, -totalSize);
	}
}

void MemoryDriver_Detect(struct MemoryDriver* self)
{
	if (!self->MultibootInfo)
	{
		if (self->Debug) 
		{
			printf("Memory: Multiboot Info Not Found!\n");
		}
		return;
	}

	struct multiboot_mmap_entry* mmap = 0;
	if (self->MultibootInfo->flags & MULTIBOOT_INFO_MEMORY)
	{
		uint32_t mem_lower = (uint32_t)self->MultibootInfo->mem_lower;
		uint32_t mem_upper = (uint32_t)self->MultibootInfo->mem_upper;
		if (self->Debug) 
		{
            printf("Memory: Upper %d KB Lower %d KB\n",mem_lower,mem_upper);
		}
	}

	if (self->MultibootInfo->flags & MULTIBOOT_INFO_MEM_MAP)
	{
		if (self->Debug) 
		{
			printf("MMap Addr: 0x%x, MMap Length: 0x%x \n",
               self->MultibootInfo->mmap_addr,self->MultibootInfo->mmap_length);
		}

		int physicalAreaIndex = 0;

		for
		(
			mmap = (struct multiboot_mmap_entry*)self->MultibootInfo->mmap_addr;
			(uint32_t)mmap < (self->MultibootInfo->mmap_addr + self->MultibootInfo->mmap_length);
			mmap = (struct multiboot_mmap_entry*)((uint32_t)mmap + mmap->size + sizeof(mmap->size))
		)
		{
			if (physicalAreaIndex > MEMORY_PHYSICAL_AREA_TABLE_SIZE)
			{
				break;
			}

			self->PhysicalAreas[physicalAreaIndex].Address = (uint32_t)mmap->addr;
			self->PhysicalAreas[physicalAreaIndex].Length = (uint32_t)mmap->len;
			self->PhysicalAreas[physicalAreaIndex].Type = (uint32_t)mmap->type;

			printf("Memory: Area %d: 0x%x %dKB (%dMB) Type %d",
				physicalAreaIndex, 
				self->PhysicalAreas[physicalAreaIndex].Address,
				self->PhysicalAreas[physicalAreaIndex].Length/1024,
				self->PhysicalAreas[physicalAreaIndex].Length/(1024*1024),
				self->PhysicalAreas[physicalAreaIndex].Type
			);

			if (self->PhysicalAreas[physicalAreaIndex].Address == MEMORY_UPPER_RAM_BASE)
			{
				printf(" <-- HEAP");
				self->BaseAddress = MEMORY_UPPER_RAM_BASE + MEMORY_UPPER_RAM_OFFSET;
				self->UpperRamSize = self->PhysicalAreas[physicalAreaIndex].Length - MEMORY_UPPER_RAM_OFFSET;
			}
         	printf("\n");
			physicalAreaIndex++;
		}
	}
	if (self->Debug) 
	{
		printf("Memory: Heap starts at 0x%x \n",self->BaseAddress);
		printf("Memory: Memory_BlockHeader header is %d bytes\n",sizeof(struct Memory_BlockHeader));
	}
}

void MemoryDriver_SetMultibootInfo(struct MemoryDriver* self, multiboot_info_t* mbi)
{
	self->MultibootInfo = mbi;
}

struct Memory_BlockCount MemoryDriver_CountUsedBlocks(struct MemoryDriver* self)
{
	struct Memory_BlockCount c;
	c.mBlocksUsed = 0;
	c.mSizeInBytes = 0;
	struct Memory_BlockHeader* current = (struct Memory_BlockHeader*)self->BlocksBegin;
	do
	{
		if (current->mInUse)
		{
			c.mBlocksUsed++;
			c.mSizeInBytes += sizeof(struct Memory_BlockHeader)+current->mSize;
		}
		current = (struct Memory_BlockHeader*)current + sizeof(struct Memory_BlockHeader) + current->mSize;
	}
	while (current < (struct Memory_BlockHeader*)self->HeapEndAddress);
	return c;
}

uint32_t MemoryDriver_GetLastBlock(struct MemoryDriver* self)
{
	struct Memory_BlockHeader* last = Memory_GetPreviousBlock(self, (struct Memory_BlockHeader*)self->HeapEndAddress);
	return (uint32_t)last;
}

void* MemoryDriver_GetPhysicalAddress(struct MemoryDriver* self, void* virtualAddress)
{
	return 0;
}

void MemoryDriver_MapPage(struct MemoryDriver* self, void* physicalAddress, void* virtualAddress, unsigned int flags)
{
}