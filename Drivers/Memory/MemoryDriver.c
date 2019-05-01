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

struct MemoryBlockHeader* MemoryDriver_FindFreeBlock(struct MemoryDriver* self, uint32_t requested_size)
{
	if (self->Debug) 
	{
		printf("Memory: Finding a free block\n");
	}

	struct MemoryBlockHeader* previous_block = 0;
	struct MemoryBlockHeader* current_block = self->StartBlock;

	// StartBlock
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
			printf("Memory: Allocated the StartBlock at 0x%x data size 0x%x, total 0x%x\n",
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
				printf("Memory: Block 0x%x has been free'd\n",(uint32_t)current_block);
			}
			// Check if the current block satisfies size requirement
			if (current_block->Size >= requested_size)
			{
				if (self->Debug) 
				{
					printf("Memory: Found free'd block of size 0x%x for allocation of new size 0x%x at 0x%x\n" ,current_block->Size, requested_size, (uint32_t)current_block);
				}
				current_block->InUse = true;
				return current_block;
			}
			// Check for unused neighbors
			else
			{
				uint32_t space_available = MemoryDriver_CheckForUnusedNeighbors(self,current_block);	
				if (space_available >= requested_size)
				{
					// Resize
					current_block->Size = space_available;
					current_block->InUse = true;
					// Fix links
					struct MemoryBlockHeader* block_after_space = (struct MemoryBlockHeader*) (((uint32_t)current_block) + space_available);
					current_block->Next = block_after_space;
					// Return
					return current_block;
				}
				// No suitable space found
				if (self->Debug) 
				{
					printf("Memory: No contiguous blocks with 0x%x bytes found\n",requested_size);
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
			printf("Memory: Reached end of heap, created new block at 0x%x, of size 0x%x, Totalling 0x%x\n",
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

uint32_t MemoryDriver_CheckForUnusedNeighbors(struct MemoryDriver* self, struct MemoryBlockHeader* header)
{
	if (self->Debug) 
	{
		printf("Memory: Checking 0x%x for unused neighbors\n" ,(uint32_t)header);
	}
	uint32_t cumulative_size = header->Size;
	struct MemoryBlockHeader* next = header->Next;
	// Next block exists and is not in use
	while (next && !next->InUse)
	{
		cumulative_size += sizeof(struct MemoryBlockHeader) + next->Size;
		next = next->Next;
	}

	if (self->Debug) 
	{
		printf("Memory: Found cumulative free space for 0x%x = 0x%x\n" ,(uint32_t)header,cumulative_size);
	}
	return cumulative_size;
}

void* MemoryDriver_Allocate(struct MemoryDriver* self, uint32_t size)
{
	if (self->Debug) 
	{
		printf("Memory: Allocate requested for size 0x%x\n", size);
	}
	struct MemoryBlockHeader* nextFree = MemoryDriver_FindFreeBlock(self, size);
	return (void*)((uint32_t)nextFree)+sizeof(struct MemoryBlockHeader);
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
*/
void* MemoryDriver_Reallocate(struct MemoryDriver* self, void *ptr, uint32_t requested_size)
{
	if (self->Debug) 
	{
		if (self->Debug) printf("Memory: Reallocate of area 0x%x requested for size 0x%x\n", (uint32_t)ptr, requested_size);
	}

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
		if (self->Debug) printf("Memory: Realloc error - memory was not previously allocated\n");
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
		return (void*) ((uint32_t)header)+sizeof(struct MemoryBlockHeader);
	}

	// Can the block be expanded
	uint32_t available_space = MemoryDriver_CheckForUnusedNeighbors(self,header);
	if (available_space >= requested_size)
	{
		if (self->Debug) printf("Memory: There is enough available space to resize the block\n");
		header->Size = available_space;
		return (void*) ((uint32_t)header)+sizeof(struct MemoryBlockHeader);
	}

	// Case b
	if (self->Debug)
	{
		printf("MemoryDriver: Could not expand/contract block, allocating new\n");
	}
	void* new_block = MemoryDriver_Allocate(self,requested_size);
	memcpy(new_block,ptr,requested_size);
	MemoryDriver_Free(self,ptr);
	return new_block;
}

struct MemoryBlockHeader* MemoryDriver_GetHeaderFromValuePointer(void* value)
{
	return (struct MemoryBlockHeader*) (((uint32_t)value) - sizeof(struct MemoryBlockHeader));
}

void MemoryDriver_Free(struct MemoryDriver* self, void* addr)
{
	struct MemoryBlockHeader* header = MemoryDriver_GetHeaderFromValuePointer(addr);

	if (self->Debug) 
	{
		printf("Memory: Freeing Memory Block at 0x%x, size 0x%x\n", (uint32_t)header,header->Size);
	}
	header->InUse = 0;
}

void MemoryDriver_Detect(struct MemoryDriver* self)
{
	if (!self->MultibootInfo)
	{
		if (self->Debug) 
		{
			printf("Memory: Fatal Error - Multiboot Info Not Found!\n");
			abort();
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

void MemoryDriver_TestAllocate(struct MemoryDriver* self) 
{
	printf("\nMemoryTest: Testing Allocate ====================\n");

	void *ptr1 = 0;
	uint32_t size1 = 1024;
	ptr1 = MemoryDriver_Allocate(self, size1);
	if (!ptr1)
	{
		printf("MemoryTest: Error - ptr_1 is null after allocation\n");
		abort();
	}
	printf("MemoryTest: ptr_1 is set after allocation 0x%x\n",(uint32_t)ptr1);

	void *ptr2 = 0;
	uint32_t size2 = 1024*2;
	ptr2 = MemoryDriver_Allocate(self, size2);
	if (!ptr2)
	{
		printf("MemoryTest: Error - ptr_2 is null after allocation\n");
		abort();
	}
	printf("MemoryTest: ptr_2 is set after allocation 0x%x\n",(uint32_t)ptr2);

	void *ptr3 = 0;
	uint32_t size3 = 1024*3;
	ptr3 = MemoryDriver_Allocate(self, size3);
	if (!ptr3)
	{
		printf("MemoryTest: Error - ptr_3 is null after allocation\n");
		abort();
	}
	printf("MemoryTest: ptr_3 is set after allocation 0x%x\n",(uint32_t)ptr3);

	struct MemoryBlockHeader *ptr1header = MemoryDriver_GetHeaderFromValuePointer(ptr1);
	
	if (!ptr1header->InUse)
	{
		printf("MemoryTest: ptr_1 is not marked InUse\n");
		abort();
	}

	if (ptr1header->Size != size1)
	{
		printf("MemoryTest: ptr_1 size does not match\n");
		abort();

	}

	printf("MemoryTest: ptr_1 tests passed\n");

	struct MemoryBlockHeader *ptr2header = MemoryDriver_GetHeaderFromValuePointer(ptr2);

	if (!ptr2header->InUse)
	{
		printf("MemoryTest: ptr_2 is not marked InUse\n");
		abort();
	}

	if (ptr2header->Size != size2)
	{
		printf("MemoryTest: ptr_2 size does not match\n");
		abort();

	}

	printf("MemoryTest: ptr_2 tests passed\n");

	struct MemoryBlockHeader *ptr3header = MemoryDriver_GetHeaderFromValuePointer(ptr3);
	if (!ptr3header->InUse)
	{
		printf("MemoryTest: ptr_3 is not marked InUse\n");
		abort();
	}

	if (ptr3header->Size != size3)
	{
		printf("MemoryTest: ptr_3 size does not match\n");
		abort();
	}

	printf("MemoryTest: ptr_3 tests passed\n");
	MemoryDriver_Free(self, ptr1);
	MemoryDriver_Free(self, ptr2);
	MemoryDriver_Free(self, ptr3);
}

void MemoryDriver_TestReallocate(struct MemoryDriver* self) 
{
	printf("\nMemoryTest: Testing Reallocate ====================\n");

	// Test Case a) reallocate with a smaller block size (contract)
	void* ptr1 = 0;
	ptr1 = MemoryDriver_Allocate(self, 1024);
	if (!ptr1)
	{
		printf("MemoryTest: Error - ptr_1 is null after allocation\n");
		abort();
	}
	printf("MemoryTest: ptr_1 is set after allocation 0x%x\n",(uint32_t)ptr1);

	printf("MemoryTest: Testing smaller reallocation\n");
	void* ptr1smaller = 0;
	ptr1smaller = MemoryDriver_Reallocate(self, ptr1, 512);

	if (!ptr1smaller)
	{
		printf("MemoryTest: Reallocating to smaller pointer returned null ptr\n");
		abort();
	}

	if (ptr1 != ptr1smaller)
	{
		printf("MemoryTest: Reallocation did not use the same pointer for a smaller block\n");
		abort();
	}

	MemoryDriver_Free(self,ptr1);

	// Test Case a) reallocate with a larger block size (expand)
	void* ptr2 = MemoryDriver_Allocate(self, 1024);
	void* ptr2b2 = MemoryDriver_Allocate(self, 1024);
	if (!ptr2)
	{
		printf("MemoryTest: Error - ptr_2 is null after allocation\n");
		abort();
	}
	printf("MemoryTest: ptr_2 is set after allocation 0x%x\n",(uint32_t)ptr2);
	printf("MemoryTest: Testing larger reallocation\n");

	void* ptr2larger = 0;
	ptr2larger = MemoryDriver_Reallocate(self,ptr2, 2048);
	if (!ptr2larger)
	{
		printf("MemoryTest: Reallocating to larger pointer returned null ptr\n");
		abort();
	}

	if (ptr2 == ptr2larger)
	{
		printf("MemoryTest: Reallocate larger returned the same pointer.\n");
		printf("            Thiss hould not happen as no neighbors are free.\n");
		abort();
	}

	struct MemoryBlockHeader* ptr2header = MemoryDriver_GetHeaderFromValuePointer(ptr2);
	if (ptr2header->InUse)
	{
		printf("MemoryTeset: ptr_2_header should have been marked free after failed realloc.\n");
		abort();
	}

	MemoryDriver_Free(self, ptr2b2);
	MemoryDriver_Free(self, ptr2larger);

	// Test case a) realloc with larger block size and available neighbors 
	void* ptr3blk1 = MemoryDriver_Allocate(self, 512);
	void* ptr3blk2 = MemoryDriver_Allocate(self, 512);
	void* ptr3blk3 = MemoryDriver_Allocate(self, 512);
	MemoryDriver_Free(self, ptr3blk2);
	MemoryDriver_Free(self, ptr3blk3);
	void* ptr3blk4 = MemoryDriver_Reallocate(self,ptr3blk1, 1450);
	struct MemoryBlockHeader* ptr3blk2header = MemoryDriver_GetHeaderFromValuePointer(ptr3blk2);
	struct MemoryBlockHeader* ptr3blk3header = MemoryDriver_GetHeaderFromValuePointer(ptr3blk2);

	if (ptr3blk2header->InUse)
	{
		printf("MemoryTest: ptr3blk2 was not freed\n");
		abort();
	}

	if (ptr3blk3header->InUse)
	{
		printf("MemoryTest: ptr3blk3 was not freed\n");
		abort();
	}

	if (ptr3blk1 != ptr3blk4)
	{
		printf("MemoryTest: Reallocation with available neighbors didn't use the same block\n");
		abort();
	}

	MemoryDriver_Free(self, ptr3blk1);
	printf("MemoryTest: Reallocation tests passed\n");
}

void MemoryDriver_TestFree(struct MemoryDriver* self) 
{
	printf("\nMemoryTest: Testing Free ====================\n");

	void *ptr_1 = 0;
	ptr_1 = MemoryDriver_Allocate(self, 1024);
	if (!ptr_1)
	{
		printf("MemoryTest: Error - ptr_1 is null after allocation\n");
		abort();
	}
	printf("MemoryTest: ptr_1 is set after allocation 0x%x\n",(uint32_t)ptr_1);

	void *ptr_2 = 0;
	ptr_2 = MemoryDriver_Allocate(self, 1024*2);
	if (!ptr_2)
	{
		printf("MemoryTest: Error - ptr_2 is null after allocation\n");
		abort();
	}
	printf("MemoryTest: ptr_2 is set after allocation 0x%x\n",(uint32_t)ptr_2);

	MemoryDriver_Free(self, ptr_1);

	void *ptr_3 = 0;
	ptr_3 = MemoryDriver_Allocate(self, 1024*3);
	if (!ptr_3)
	{
		printf("MemoryTest: Error - ptr_3 is null after allocation\n");
		abort();
	}
	printf("MemoryTest: ptr_3 is set after allocation 0x%x\n",(uint32_t)ptr_3);

	MemoryDriver_Free(self, ptr_2);
	MemoryDriver_Free(self, ptr_3);

	struct MemoryBlockHeader *ptr_1_header = MemoryDriver_GetHeaderFromValuePointer(ptr_1);
	struct MemoryBlockHeader *ptr_2_header = MemoryDriver_GetHeaderFromValuePointer(ptr_2);
	struct MemoryBlockHeader *ptr_3_header = MemoryDriver_GetHeaderFromValuePointer(ptr_3);

	if (!ptr_1_header->InUse)
	{
		printf("MemoryTest: ptr_1 freed successfully\n");
	}
	else
	{
		printf("MemoryTest: ptr_1 WAS NOT freed successfully\n");
		abort();
	}

	if (!ptr_2_header->InUse)
	{
		printf("MemoryTest: ptr_2 freed successfully\n");
	}
	else
	{
		printf("MemoryTest: ptr_2 WAS NOT freed successfully\n");
		abort();
	}

	if (!ptr_3_header->InUse)
	{
		printf("MemoryTest: ptr_3 freed successfully\n");
	}
	else
	{
		printf("MemoryTest: ptr_3 WAS NOT freed successfully\n");
		abort();
	}

	printf("MemoryTest: Free tests passed\n");	
}