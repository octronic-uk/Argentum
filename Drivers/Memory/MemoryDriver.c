#include <Drivers/Memory/MemoryDriver.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Objects/Kernel/Kernel.h>

extern struct Kernel _Kernel;
extern void* endKernel;

bool MemoryDriver_Constructor(struct MemoryDriver* self)
{
	self->MultibootInfo = _Kernel.MultibootInfo;
	self->Debug = false;
	self->HeapSize = 0;
	self->HeapBaseAddress = 0;
	self->StartBlock = 0;
	MemoryDriver_Detect(self);
	return true;
}

struct MemoryBlockHeader* MemoryDriver_ClaimFreeBlock(struct MemoryDriver* self, uint32_t requested_size)
{
	if (self->Debug) 
	{
		printf("Memory: Finding a free block\n");
	}

	struct MemoryBlockHeader* previous_block = 0;
	struct MemoryBlockHeader* current_block = self->StartBlock;

	// StartBlock, unused
	if (current_block == self->StartBlock && 
	   !current_block->Next && 
	   !current_block->InUse && 
	   !current_block->Size)
	{
		current_block->InUse = true;
		current_block->Size = requested_size;
		current_block->Next = 0; 

		if (self->Debug)
		{
			printf("Memory: Allocated the StartBlock at 0x%x data size %d, total 0x%x\n",
					(uint32_t)current_block, requested_size, 
					sizeof(struct MemoryBlockHeader)+requested_size);
		}
		return current_block;
	}

	while(current_block)
	{
		if (self->Debug)
		{
			printf("Memory: Checking Block 0x%x\n",(uint32_t)current_block);
		}

		// Header in Use
		if (current_block->InUse)
		{
			if (self->Debug)
			{
				printf("Memory: Block 0x%x is in use moving to next...\n",(uint32_t)current_block);
			}
			previous_block = current_block;
			current_block = current_block->Next;
		}
		// Previously free'd block
		else
		{
			if (self->Debug)
			{
				printf("Memory: blk=0x%x sz=%d has been free'd\n",(uint32_t)current_block, current_block->Size);
			}
			// Check if the current block satisfies size requirement
			if (current_block->Size >= requested_size)
			{
				if (self->Debug) 
				{
					printf("Memory: Found free'd block of size %d for allocation of new size %d at 0x%x\n" ,current_block->Size, requested_size, (uint32_t)current_block);
				}
				current_block->Size = requested_size;
				current_block->InUse = true;
				MemoryDriver_InsertDummyBlock(self,current_block);
				return current_block;
			}
			// Check for unused neighbors
			else
			{
				uint32_t space_available = MemoryDriver_CheckForUnusedNeighbors(self,current_block,requested_size);	
				if (self->Debug) printf("Memory: %d bytes found in contiguous free blocks\n",space_available);
				if (space_available >= requested_size)
				{
					// Resize
					current_block->Size = requested_size;
					current_block->InUse = true;
					

					void* mem_area = (void*) ((uint32_t)current_block) + sizeof(struct MemoryBlockHeader);
					memset(mem_area, 0, sizeof(struct MemoryBlockHeader)+space_available);
					// Fix links

					MemoryDriver_InsertDummyBlock(self,current_block);

/*
					struct MemoryBlockHeader* block_after_space = (struct MemoryBlockHeader*) (
						((uint32_t)current_block) +
						sizeof(struct MemoryBlockHeader) + 
						space_available
					);
					current_block->Next = block_after_space;
					*/
					// Return
					return current_block;
				}
				// No suitable space found
				if (self->Debug) 
				{
					printf("Memory: No contiguous blocks with %d bytes found\n",requested_size);
				}
				current_block = current_block->Next;
			}
		}
	}

	// Should always be true
	if (previous_block)
	{
		current_block = (struct MemoryBlockHeader*) ( ((uint32_t)previous_block) + sizeof(struct MemoryBlockHeader) + previous_block->Size );
		// Set current to area after the last block
		current_block->Size = requested_size;
		current_block->InUse = true;
		current_block->Next = 0;
		previous_block->Next = current_block;
		if (self->Debug)
		{
			printf("Memory: Reached end of heap, created new block at 0x%x, of size=%d, Totalling=%d\n",
				(uint32_t)current_block, 
				requested_size, sizeof(struct MemoryBlockHeader)+requested_size);
		}
	}
	else
	{
		printf("Memory: Fatal Error - No first block found\n");
		abort();
		current_block = 0;
	}

	return current_block;
}

struct MemoryBlockHeader* MemoryDriver_InsertDummyBlock(struct MemoryDriver* self, struct MemoryBlockHeader* resized_header)
{
	struct MemoryBlockHeader* next = resized_header->Next;

	// No next block to resize to
	if (!next)
	{
		if (self->Debug) printf("Memory: Can't insert dummy at the end of the heap\n");
		return 0;
	}

	uint32_t space_begins = ((uint32_t)resized_header) + sizeof(struct MemoryBlockHeader) + resized_header->Size;
	uint32_t space_ends = ((uint32_t)next);
	int32_t space_available = space_ends - space_begins;
	int32_t dummy_data_size = space_available - sizeof(struct MemoryBlockHeader);


	if (dummy_data_size > 0)
	{
		if (self->Debug) printf("Memory: Making dummy at 0x%x in space of %d\n",space_begins, space_available);
		struct MemoryBlockHeader* dummy_block = (struct MemoryBlockHeader*) ((uint32_t)space_begins);
		dummy_block->InUse = 0;
		dummy_block->Size = dummy_data_size;

		resized_header->Next = dummy_block;
		dummy_block->Next = (struct MemoryBlockHeader*)space_ends;

		if (self->Debug) printf("Memory: Inserted a dummy block blk=0x%x sz=%d\n",(uint32_t)dummy_block, dummy_data_size);
		return dummy_block;
	}

	if (self->Debug) printf("Memory: Not enough space to insert dummy block\n");
	
	return 0;
}

uint32_t MemoryDriver_CheckForUnusedNeighbors(struct MemoryDriver* self, struct MemoryBlockHeader* header, uint32_t requested_size)
{
	if (self->Debug) 
	{
		printf("Memory: Checking 0x%x for unused neighbors\n" ,(uint32_t)header);
	}
	uint32_t cumulative_size = header->Size;
	struct MemoryBlockHeader* next = header->Next;
	// Next block exists and is not in use
	while (next && !next->InUse && cumulative_size < requested_size)
	{
		cumulative_size += sizeof(struct MemoryBlockHeader) + next->Size;
		next = next->Next;
	}

	if (self->Debug) 
	{
		printf("Memory: Found cumulative free space for hdr=0x%x sz=%d\n" ,(uint32_t)header,cumulative_size);
	}
	return cumulative_size;
}

void* MemoryDriver_Allocate(struct MemoryDriver* self, uint32_t size)
{
	if (self->Debug) 
	{
		printf("Memory: Allocate requested for size %d\n", size);
	}

	// Ignore zero allocations
	if (!size) return 0; 

	uint32_t nextFree = (uint32_t)MemoryDriver_ClaimFreeBlock(self, size);
	uint32_t mem = nextFree + sizeof(struct MemoryBlockHeader);
	if (self->Debug) printf("Memory: Allocated block=0x%x mem=0x%x\n",nextFree, mem);
	return (void*) mem;
}


/*
	Reallocates the given area of memory. It must be previously allocated by malloc(), calloc() or 
	realloc() and not yet freed with a call to free or realloc. Otherwise, the results are undefined.

	The reallocation is done by either:

	a) expanding or contracting the existing area pointed to by ptr, if possible. The contents of 
	   the area remain unchanged up to the lesser of the new and old sizes. If the area is expanded, 
	   the contents of the new part of the array are undefined.

	b) allocating a new memory block of size new_size bytes, copying memory area with size equal 
	   the lesser of the new and the old sizes, and freeing the old block.

	c) If there is not enough memory, the old memory block is not freed and null pointer is returned.

	d) If ptr is NULL, the behavior is the same as calling malloc(new_size).

	e) If new_size is zero, the behavior is implementation defined (null pointer may be returned 
	   (in which case the old memory block may or may not be freed), or some non-null pointer may be 
	   returned that may not be used to access storage).


	NOTE:
		New header should only take up requested size and a dummy block inserted inbetween to 
		accomodate reallocation of §the remaining space.
*/
void* MemoryDriver_Reallocate(struct MemoryDriver* self, void *ptr, uint32_t requested_size)
{
	if (self->Debug) 
		printf("Memory: Reallocate  area=0x%x requested for sz=%d\n", (uint32_t)ptr, requested_size);

	// Case d
	if (!ptr)
	{
		if (self->Debug) printf("Memory: Realloc pointer is 0, calling MemoryDriver_Allocate\n");
		return MemoryDriver_Allocate(self,requested_size);
	}

	// case e
	if (!requested_size)
	{
		if (self->Debug) printf("Memory: Realloc error - requested size is 0\n");
		return 0;
	}

	struct MemoryBlockHeader* header = MemoryDriver_GetHeaderFromValuePointer(ptr);

	// Error case Not allocated or previously freed
	if (!header->InUse)
	{
		if (self->Debug) printf("Memory: Realloc error - memory was not allocated or previously free'd\n");
		return 0;
	}

	// Case a

	// Size meets requirements
	if (header->Size >= requested_size)
	{
		if (self->Debug)
		{
			printf("Memory: Realloc, block will accomodate reallocation\n");
		}
		header->InUse = true;
		header->Size = requested_size;
		MemoryDriver_InsertDummyBlock(self,header);
		return (void*) ((uint32_t)header)+sizeof(struct MemoryBlockHeader);
	}

	// Can the block be expanded?
	uint32_t available_space = MemoryDriver_CheckForUnusedNeighbors(self,header,requested_size);
	if (available_space >= requested_size)
	{
		if (self->Debug) printf("Memory: There is enough available space to resize the block\n");
		header->Size = requested_size;
		header->InUse = true;
		header->Next = (struct MemoryBlockHeader*)(((uint32_t)header)+available_space);
		MemoryDriver_InsertDummyBlock(self,header);
		return (void*) ((uint32_t)header)+sizeof(struct MemoryBlockHeader);
	}


	// Case b
	if (self->Debug)
	{
		printf("MemoryDriver: Could not expand/contract block, allocating new\n");
	}

	struct MemoryBlockHeader* old_block_header = MemoryDriver_GetHeaderFromValuePointer(ptr);
	uint32_t old_size = old_block_header->Size;
	uint32_t size_to_copy = old_size < requested_size ? old_size : requested_size;
	void* new_block = MemoryDriver_Allocate(self, requested_size);
	memcpy(new_block, ptr, size_to_copy);
	MemoryDriver_Free(self, ptr);
	return new_block;
}

struct MemoryBlockHeader* MemoryDriver_GetHeaderFromValuePointer(void* value)
{
	return (struct MemoryBlockHeader*) (((uint32_t)value) - sizeof(struct MemoryBlockHeader));
}

void MemoryDriver_Free(struct MemoryDriver* self, void* addr)
{
	struct MemoryBlockHeader* header = MemoryDriver_GetHeaderFromValuePointer(addr);

	if (!header->InUse)
	{
		printf("Memory: Attempted to double free block header=0x%x addr=0x%x\n",(uint32_t)header,(uint32_t)addr);
		abort();
	}

	if (self->Debug) 
	{
		printf("Memory: Freeing Memory hdr=0x%x, block=0x%x, size=%`d\n",(uint32_t)header,(uint32_t)addr,header->Size);
	}
	header->InUse = 0;
}

void MemoryDriver_Detect(struct MemoryDriver* self)
{
	if (!self->MultibootInfo)
	{
		printf("Memory: Fatal Error - Multiboot Info Not Found!\n");
		abort();
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
		bool heapSet = false;
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

			if (!heapSet && self->PhysicalAreas[physicalAreaIndex].Address == MEMORY_UPPER_RAM_BASE)
			{
				printf(" <-- KERNEL");
				// Align to 0x100000
				uint32_t align = (0x100000 - ((uint32_t)&endKernel % 0x100000));
				self->HeapBaseAddress = (uint32_t)&endKernel + align;
				self->HeapSize = (MEMORY_UPPER_RAM_BASE + self->PhysicalAreas[physicalAreaIndex].Length) - ((uint32_t)&endKernel + align) ;
				self->StartBlock = (struct MemoryBlockHeader*) self->HeapBaseAddress;
				// Clear start block
				self->StartBlock->Size = 0;
				self->StartBlock->InUse = 0;
				self->StartBlock->Next = 0;
				heapSet = true;
			}
         	printf("\n");
			physicalAreaIndex++;
		}
	}
	if (self->Debug) 
	{
		printf("Memory: End of kernel at 0x%x\n",(uint32_t)&endKernel);
		printf("Memory: Heap starts at 0x%x \n",self->HeapBaseAddress);
		printf("Memory: Memory_BlockHeader header is %d bytes\n",sizeof(struct MemoryBlockHeader));
	}
}

void MemoryDriver_SetMultibootInfo(struct MemoryDriver* self, multiboot_info_t* mbi)
{
	self->MultibootInfo = mbi;
}

void MemoryDriver_PrintMemoryMap(struct MemoryDriver* self)
{
	printf("Memory: Memory Map =========================================\n\n");
	struct MemoryBlockHeader* block = self->StartBlock;
	while(block)
	{
		printf("\tBlock=0x%x\tSz=%d\tInUse=%d\tNext=0x%x\n",block,block->Size,block->InUse,block->Next);	
		block = block->Next;
	}
	printf("\n============================================================\n");
}