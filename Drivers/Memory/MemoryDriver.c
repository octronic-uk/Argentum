#include <Drivers/Memory/MemoryDriver.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;
extern void* endKernel;

bool MemoryDriver_Constructor(MemoryDriver* self)
{
	self->MultibootInfo = _Kernel.MultibootInfo;
	self->Debug = false;
	self->HeapSize = 0;
	self->HeapBaseAddress = 0;
	self->StartBlock = 0;
	MemoryDriver_Detect(self);
	return true;
}

MemoryBlockHeader* MemoryDriver_ClaimFreeBlock(MemoryDriver* self, uint32_t requested_size)
{
	if (self->Debug) 
	{
		printf("Memory: Finding a free block\n");
	}

	MemoryBlockHeader* previous_block = 0;
	MemoryBlockHeader* current_block = self->StartBlock;

	// StartBlock, unused
	if (current_block == self->StartBlock && 
	   !current_block->Next && 
	   !current_block->InUse)
	{
		current_block->InUse = true;
		current_block->Size = requested_size;
		current_block->Next = 0; 

		if (self->Debug)
		{
			printf("Memory: Allocated the StartBlock at 0x%x data size %d, total %d\n",
					(uint32_t)current_block, requested_size, 
					sizeof(MemoryBlockHeader)+requested_size);
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
				uint32_t old_size = current_block->Size;
				current_block->Size = requested_size;
				current_block->InUse = true;
				// Restore old size
				if (!MemoryDriver_InsertDummyBlock(self,current_block))
				{
					current_block->Size = old_size;
				}
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
					uint32_t old_size = current_block->Size;
					current_block->Size = requested_size;
					current_block->InUse = true;
					current_block->Next = (MemoryBlockHeader*)(((uint32_t)current_block) + sizeof(MemoryBlockHeader) + space_available);

					void* mem_area = (void*) ((uint32_t)current_block) + sizeof(MemoryBlockHeader);
					memset(mem_area, 0, sizeof(MemoryBlockHeader)+space_available);

					if (!MemoryDriver_InsertDummyBlock(self,current_block))
					{
						if (current_block != self->StartBlock)
						{
							current_block->Size = old_size;
						}
					}

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
		current_block = (MemoryBlockHeader*) ( ((uint32_t)previous_block) + sizeof(MemoryBlockHeader) + previous_block->Size );
		// Set current to area after the last block
		current_block->Size = requested_size;
		current_block->InUse = true;
		current_block->Next = 0;
		previous_block->Next = current_block;
		if (self->Debug)
		{
			printf("Memory: Reached end of heap, created new block at 0x%x, of size=%d, Totalling=%d\n",
				(uint32_t)current_block, 
				requested_size, sizeof(MemoryBlockHeader)+requested_size);
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

MemoryBlockHeader* MemoryDriver_InsertDummyBlock(MemoryDriver* self, MemoryBlockHeader* resized_header)
{
	if (self->Debug) printf("Memory: Attempting to insert dummy block\n");
	MemoryBlockHeader* next = resized_header->Next;

	if (self->Debug) printf("Memory: next block is now 0x%x\n",(uint32_t)next);
	// No next block to resize to
	if (!next)
	{
		if (self->Debug) printf("Memory: Can't insert dummy at the end of the heap\n");
		return 0;
	}

	uint32_t space_begins = ((uint32_t)resized_header) + sizeof(MemoryBlockHeader) + resized_header->Size;
	uint32_t space_ends = ((uint32_t)next);
	int32_t space_available = space_ends - space_begins;
	int32_t dummy_data_size = space_available - sizeof(MemoryBlockHeader);

	if (dummy_data_size > 0)
	{
		if (self->Debug) printf("Memory: Making dummy at 0x%x in space of %d\n",space_begins, space_available);
		MemoryBlockHeader* dummy_block = (MemoryBlockHeader*) ((uint32_t)space_begins);
		dummy_block->InUse = 0;
		dummy_block->Size = dummy_data_size;

		resized_header->Next = dummy_block;
		dummy_block->Next = (MemoryBlockHeader*)space_ends;

		if (self->Debug) printf("Memory: Inserted a dummy block blk=0x%x sz=%d\n",(uint32_t)dummy_block, dummy_data_size);
		return dummy_block;
	}

	if (self->Debug) printf("Memory: Not enough space to insert dummy block (%d)\n",dummy_data_size);
	return 0;
}

uint32_t MemoryDriver_CheckForUnusedNeighbors(MemoryDriver* self, MemoryBlockHeader* header, uint32_t requested_size)
{
	if (self->Debug) 
	{
		printf("Memory: Checking 0x%x for unused neighbors\n" ,(uint32_t)header);
	}
	uint32_t cumulative_size = header->Size;
	MemoryBlockHeader* next = header->Next;
	// Next block exists and is not in use
	while (next && !next->InUse && cumulative_size < requested_size)
	{
		cumulative_size += sizeof(MemoryBlockHeader) + next->Size;
		next = next->Next;
	}

	if (self->Debug) 
	{
		printf("Memory: Found cumulative free space for hdr=0x%x sz=%d\n" ,(uint32_t)header,cumulative_size);
	}
	return cumulative_size;
}

void* MemoryDriver_Allocate(MemoryDriver* self, uint32_t size)
{
	if (self->Debug) 
	{
		printf("Memory: Allocate requested for size %d\n", size);
	}

	// Ignore zero allocations
	if (!size) return 0; 

	uint32_t nextFree = (uint32_t)MemoryDriver_ClaimFreeBlock(self, size);
	uint32_t mem = nextFree + sizeof(MemoryBlockHeader);
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
void* MemoryDriver_Reallocate(MemoryDriver* self, void *ptr, uint32_t requested_size)
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

	MemoryBlockHeader* header = MemoryDriver_GetHeaderFromValuePointer(self,ptr);

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
		uint32_t old_size = header->Size;
		header->InUse = true;
		header->Size = requested_size;
		if (!MemoryDriver_InsertDummyBlock(self,header))
		{
			header->Size = old_size;
		}
		return (void*) ((uint32_t)header)+sizeof(MemoryBlockHeader);
	}

	// Is this at the end of the heap
	if (!header->Next)
	{
		if (self->Debug) printf("Memory: Block is at the end of the heap, resizing\n");
		header->Size = requested_size;
		return ptr;
	}

	// Can the block be expanded?
	uint32_t available_space = MemoryDriver_CheckForUnusedNeighbors(self,header,requested_size);
	if (available_space >= requested_size)
	{
		if (self->Debug) printf("Memory: There is enough available space to resize the block\n");
		header->Size = requested_size;
		header->InUse = true;
		header->Next = (MemoryBlockHeader*)(((uint32_t)header) + sizeof(MemoryBlockHeader) + available_space);
		uint32_t old_size = header->Size;
		if (!MemoryDriver_InsertDummyBlock(self, header))
		{
			header->Size = old_size;
		}
		return (void*) ((uint32_t)header) + sizeof(MemoryBlockHeader);
	}


	// Case b
	if (self->Debug)
	{
		printf("Memory: Could not expand/contract block, allocating new\n");
	}

	MemoryBlockHeader* old_block_header = MemoryDriver_GetHeaderFromValuePointer(self,ptr);
	uint32_t old_size = old_block_header->Size;
	uint32_t size_to_copy = old_size < requested_size ? old_size : requested_size;
	void* new_block = MemoryDriver_Allocate(self, requested_size);
	memcpy(new_block, ptr, size_to_copy);
	MemoryDriver_Free(self, ptr);
	return new_block;
}

MemoryBlockHeader* MemoryDriver_GetHeaderFromValuePointer(MemoryDriver* self, void* value)
{
	MemoryBlockHeader* retval = (MemoryBlockHeader*) (((uint32_t)value) - sizeof(MemoryBlockHeader));

	if (!MemoryDriver_IsValidBlock(self, retval))
	{
		printf("Memory: Requested an invalid block header 0x%x\n",(uint32_t)retval);
		abort();
	}
	return retval;
}

bool MemoryDriver_IsValidBlock(MemoryDriver* self, MemoryBlockHeader* header)
{
	MemoryBlockHeader* current = self->StartBlock;

	while (current) 
	{
		if (current == header) return true;
		else current = current->Next;
	}
	return false;
}

void MemoryDriver_Free(MemoryDriver* self, void* addr)
{
	MemoryBlockHeader* header = MemoryDriver_GetHeaderFromValuePointer(self,addr);

	if (!header->InUse)
	{
		printf("Memory: Attempted to double free block header=0x%x addr=0x%x\n",(uint32_t)header,(uint32_t)addr);
		abort();
	}

	if (self->Debug) 
	{
		printf("Memory: Freeing Memory hdr=0x%x, block=0x%x, size=%d\n",(uint32_t)header,(uint32_t)addr,header->Size);
	}
	header->InUse = 0;
	MemoryDriver_CleanUpHeap(self);
}

void MemoryDriver_Detect(MemoryDriver* self)
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
				uint32_t align = (MEMORY_STACK_ALIGN - ((uint32_t)&endKernel % MEMORY_STACK_ALIGN));
				self->HeapBaseAddress = (uint32_t)&endKernel + align;
				self->HeapSize = (MEMORY_UPPER_RAM_BASE + self->PhysicalAreas[physicalAreaIndex].Length) - ((uint32_t)&endKernel + align) ;
				self->StartBlock = (MemoryBlockHeader*) self->HeapBaseAddress;
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
		printf("Memory: Memory_BlockHeader header is %d bytes\n",sizeof(MemoryBlockHeader));
	}
}

void MemoryDriver_SetMultibootInfo(MemoryDriver* self, multiboot_info_t* mbi)
{
	self->MultibootInfo = mbi;
}

void MemoryDriver_CleanUpHeap(MemoryDriver* self)
{
	MemoryBlockHeader* block = self->StartBlock;
	MemoryBlockHeader* lastInUse = block;

	while (1)
	{
		if (!block) break;

		if (block->InUse)
		{
			lastInUse = block;
		}
		block = block->Next;
	}

	if (lastInUse)
	{
		lastInUse->Next = 0;
	}
}

void MemoryDriver_PrintMemoryMap(MemoryDriver* self)
{
	MemoryBlockHeader* block = self->StartBlock;

	printf("Memory Map\n");
	printf("+------------+------------+------------+----------+----------+------------+\n");
	printf("| Block      | Pointer    | Size       | Size     | In use   | Next       |\n");
	printf("+------------+------------+------------+----------+----------+------------+\n");
	while(block)
	{
		printf("| 0x%08x | 0x%08x | 0x%08x | %08d | %s | 0x%08x |\n",
			block,
			(uint32_t)block+sizeof(MemoryBlockHeader),
			block->Size,
			block->Size,
			block->InUse ? "Yes     " : "No      ",
			block->Next);	

		if (block->Next != 0 && ((uint32_t)block->Next != (uint32_t)block + sizeof(MemoryBlockHeader) + block->Size))
		{
			printf("Contiguous Memory Error!!! \n");
		}

		block = block->Next;
	}
	printf("+------------+------------+------------+----------+----------+------------+\n");
}