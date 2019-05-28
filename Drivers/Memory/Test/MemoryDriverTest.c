#include "MemoryDriverTest.h"

#include <stdlib.h>
#include <stdio.h>
#include "../MemoryDriver.h"

void MemoryDriverTest_Allocate(struct MemoryDriver* self) 
{
	printf("\nMemoryDriverTest: Testing Allocate ====================\n");

	void *ptr1 = 0;
	uint32_t size1 = 1024;
	ptr1 = MemoryDriver_Allocate(self, size1, __FILE__, __LINE__);
	if (!ptr1)
	{
		printf("MemoryDriverTest: Error - ptr_1 is null after allocation\n");
		abort();
	}
	printf("MemoryDriverTest: ptr_1 is set after allocation 0x%x\n",(uint32_t)ptr1);

	void *ptr2 = 0;
	uint32_t size2 = 2048;
	ptr2 = MemoryDriver_Allocate(self, size2, __FILE__, __LINE__);
	if (!ptr2)
	{
		printf("MemoryDriverTest: Error - ptr_2 is null after allocation\n");
		abort();
	}
	printf("MemoryDriverTest: ptr_2 is set after allocation 0x%x\n",(uint32_t)ptr2);

	void *ptr3 = 0;
	uint32_t size3 = 3072;
	ptr3 = MemoryDriver_Allocate(self, size3, __FILE__, __LINE__);
	if (!ptr3)
	{
		printf("MemoryDriverTest: Error - ptr_3 is null after allocation\n");
		abort();
	}
	printf("MemoryDriverTest: ptr_3 is set after allocation 0x%x\n",(uint32_t)ptr3);

	struct MemoryBlockHeader *ptr1header = MemoryDriver_GetHeaderFromValuePointer(self,ptr1,0);
	
	if (!ptr1header->InUse)
	{
		printf("MemoryDriverTest: ptr_1 is not marked InUse\n");
		abort();
	}

	if (ptr1header->Size != size1)
	{
		printf("MemoryDriverTest: ptr_1 size does not match\n");
		abort();

	}

	printf("MemoryDriverTest: ptr_1 tests passed\n");

	struct MemoryBlockHeader *ptr2header = MemoryDriver_GetHeaderFromValuePointer(self,ptr2,0);

	if (!ptr2header->InUse)
	{
		printf("MemoryDriverTest: ptr_2 is not marked InUse\n");
		abort();
	}

	if (ptr2header->Size != size2)
	{
		printf("MemoryDriverTest: ptr_2 size does not match\n");
		abort();

	}

	printf("MemoryDriverTest: ptr_2 tests passed\n");

	struct MemoryBlockHeader *ptr3header = MemoryDriver_GetHeaderFromValuePointer(self,ptr3,0);
	if (!ptr3header->InUse)
	{
		printf("MemoryDriverTest: ptr_3 is not marked InUse\n");
		abort();
	}

	if (ptr3header->Size != size3)
	{
		printf("MemoryDriverTest: ptr_3 size does not match\n");
		abort();
	}

	MemoryDriver_Free(self, ptr1, __FILE__, __LINE__);
	MemoryDriver_Free(self, ptr2, __FILE__, __LINE__);
	MemoryDriver_Free(self, ptr3, __FILE__, __LINE__);

	printf("MemoryDriverTest: ptr_3 tests passed\n");
    MemoryDriver_PrintMemoryMap(self);
}

// Test Case a) Reallocate with a larger block size (expand)
//              and smaller block (contract)
void MemoryDriverTest_ReallocateCaseA(struct MemoryDriver* self) 
{
	printf("\nMemoryDriverTest: Testing Reallocate A ==================\n");

	// Test Case a) reallocate with a smaller block size (contract)
	void* ptr1 = 0;
	ptr1 = MemoryDriver_Allocate(self, 1024, __FILE__, __LINE__);
	if (!ptr1)
	{
		printf("MemoryDriverTest: Reallocation A Error - ptr_1 is null after allocation\n");
		abort();
	}
	struct MemoryBlockHeader* ptr1header = MemoryDriver_GetHeaderFromValuePointer(self,ptr1,0);
	printf("MemoryDriverTest: Reallocation A ptr_1 is set after allocation 0x%x sz=%d\n",(uint32_t)ptr1, ptr1header->Size);


    MemoryDriver_PrintMemoryMap(self);

	printf("MemoryDriverTest: Reallocation A Testing smaller reallocation\n");
	void* ptr1smaller = MemoryDriver_Reallocate(self, ptr1, 512, __FILE__, __LINE__);


    MemoryDriver_PrintMemoryMap(self);

	if (!ptr1smaller)
	{
		printf("MemoryDriverTest: Reallocating to smaller pointer returned null ptr\n");
		abort();
	}

	if (ptr1 != ptr1smaller)
	{
		printf("MemoryDriverTest: Reallocation A Reallocation did not use the same pointer for a smaller block\n");
		abort();
	}

	MemoryDriver_Free(self,ptr1, __FILE__, __LINE__);
    MemoryDriver_PrintMemoryMap(self);



	printf("\nMemoryDriverTest: >>>> Testing larger reallocation\n");

	void* ptr2 = MemoryDriver_Allocate(self, 1024, __FILE__, __LINE__);
    MemoryDriver_PrintMemoryMap(self);

	// Pointer 2 block 2 will stop ptr2 being resized beyond 1024
	void* ptr2b2 = MemoryDriver_Allocate(self, 1024, __FILE__, __LINE__);
    MemoryDriver_PrintMemoryMap(self);

	if (!ptr2)
	{
		printf("MemoryDriverTest: Reallocation A Error - ptr2 is null after allocation\n");
		abort();
	}

	if (!ptr2b2)
	{
		printf("MemoryDriverTest: Reallocation A Error - ptr2b2 is null after allocation\n");
		abort();
	}

	printf("MemoryDriverTest: Reallocation A ptr2 is set after allocation 0x%x\n",(uint32_t)ptr2);
	printf("MemoryDriverTest: Reallocation A ptr2b2 is set after allocation 0x%x\n",(uint32_t)ptr2b2);


	// A size pointer 2 cannot handle
	void* ptr2larger = MemoryDriver_Reallocate(self, ptr2, 4096, __FILE__, __LINE__);

	if (!ptr2larger)
	{
		printf("MemoryDriverTest: Reallocation A Reallocating to larger pointer returned null ptr\n");
		abort();
	}

	if (ptr2 == ptr2larger)
	{
		printf("MemoryDriverTest: Reallocation A Reallocate larger returned the same pointer.\n");
		printf("            This hould not happen as no neighbors are free.\n");
		abort();
	}

	struct MemoryBlockHeader* ptr2header = MemoryDriver_GetHeaderFromValuePointer(self,ptr2,0);
	if (ptr2header->InUse)
	{
		printf("MemoryTeset: Reallocation A ptr_2_header should have been marked free after failed realloc.\n");
		abort();
	}

	MemoryDriver_Free(self, ptr2b2, __FILE__, __LINE__);
	MemoryDriver_Free(self, ptr2larger, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);

	// Test case a) realloc with larger block size and available neighbors 
	printf("\n\n>>>> Testing realloc with larger block\n");
	void* ptr3blk1 = MemoryDriver_Allocate(self, 512, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);

	void* ptr3blk2 = MemoryDriver_Allocate(self, 512, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);

	void* ptr3blk3 = MemoryDriver_Allocate(self, 512, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);

	MemoryDriver_Free(self, ptr3blk2, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);

	MemoryDriver_Free(self, ptr3blk3, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);

	void* ptr3blk4 = MemoryDriver_Reallocate(self,ptr3blk1, 1450, __FILE__, __LINE__);

	if (MemoryDriver_IsValidBlock(self, ptr3blk2))
	{
		printf("MemoryDriverTest: Reallocation A ptr3blk2 was not freed\n");
		abort();
	}

	if (MemoryDriver_IsValidBlock(self, ptr3blk3))
	{
		printf("MemoryDriverTest: Reallocation A ptr3blk3 was not freed\n");
		abort();
	}

	MemoryDriver_Free(self, ptr3blk1, __FILE__, __LINE__);

	printf("MemoryDriverTest: Reallocation A tests passed\n");
    MemoryDriver_PrintMemoryMap(self);
}

// Realloc with a larger block size and no available neighbors (allocate at end)
void MemoryDriverTest_ReallocateCaseB(struct MemoryDriver* self) 
{
	printf("\nMemoryDriverTest: Testing Reallocate B ==================\n");
	uint32_t test_size_small  = 100;
	uint32_t test_size_medium = 1000;
	uint32_t test_size_large  = 10000;

	void* ptr1 = MemoryDriver_Allocate(self,test_size_small, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);
	void* ptr2 = MemoryDriver_Allocate(self,test_size_medium, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);
	void* ptr3 = MemoryDriver_Allocate(self,test_size_small, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);

	struct MemoryBlockHeader* header1 = MemoryDriver_GetHeaderFromValuePointer(self,ptr1,0);	
	struct MemoryBlockHeader* header2 = MemoryDriver_GetHeaderFromValuePointer(self,ptr2,0);	
	struct MemoryBlockHeader* header3 = MemoryDriver_GetHeaderFromValuePointer(self,ptr3,0);	

	// Make sure the header is correct
	if (!ptr1 || !header1->InUse || header1->Size != test_size_small)
	{
		printf("MemoryDriverTest: Error - Realloc B Error 1\n");
		abort();
	}

	// Make sure the header is correct
	if (!ptr2 || !header2->InUse || header2->Size != test_size_medium)
	{
		printf("MemoryDriverTest: Error - Realloc B Error 2\n");
		abort();
	}

	// Make sure the header is correct
	if (!ptr3 || !header3->InUse || header3->Size != test_size_small)
	{
		printf("MemoryDriverTest: Error - Realloc B Error 3\n");
		abort();
	}


	// Create free block at pointer 1
	MemoryDriver_Free(self,ptr1, __FILE__, __LINE__);
	MemoryDriver_PrintMemoryMap(self);

	// Attempt to reallocate pointer 2
	void* reallocated_ptr_2 = MemoryDriver_Reallocate(self,ptr2,test_size_large, __FILE__, __LINE__);
    MemoryDriver_PrintMemoryMap(self);

	// Make sure pointer 2 was moved
	if (ptr2 == reallocated_ptr_2)
	{
		printf("MemoryDriverTest: Error Realloc B Error 2 reallocated_ptr2 was not moved\n");
		abort();
	}

	//  Make sure ptr2 was original freed
	if (header2->InUse)
	{
		printf("MemoryDriverTest: Error Realloc B Error 3 ptr2 was not freed\n");
		abort();
	}

	// make sure reallocated_ptr2 was given a block AFTER ptr2
	if (ptr2 > reallocated_ptr_2)
	{
		printf("MemoryDriverTest: Error Realloc B Error 4 reallocated block is not after ptr2\n");
        printf("            ptr2 at 0x%x, reallocated_ptr_2 at 0x%x\n",ptr2, reallocated_ptr_2);
		abort();
	}

    MemoryDriver_PrintMemoryMap(self);

	// Check reallocated block header is correct
	struct MemoryBlockHeader* reallocated_ptr_2_header = MemoryDriver_GetHeaderFromValuePointer(self,reallocated_ptr_2,0);
	if (!reallocated_ptr_2_header->InUse  ||
		reallocated_ptr_2_header->Size != test_size_large ||
		reallocated_ptr_2_header->Next != 0)
	{
		printf("MemoryDriverTest: Error Realloc B Error 5 Header is incorrect\n");	
		abort();
	}

	// Clean up
	MemoryDriver_Free(self,ptr3, __FILE__, __LINE__);
    MemoryDriver_PrintMemoryMap(self);
	MemoryDriver_Free(self,reallocated_ptr_2, __FILE__, __LINE__);
    MemoryDriver_PrintMemoryMap(self);

	printf("MemoryDriverTest: Realloc B Tests Passed!\n");
}

void MemoryDriverTest_Free(struct MemoryDriver* self) 
{
	printf("\nMemoryDriverTest: Testing Free ====================\n");
	const int n_tests = 100;
	void* pointers[n_tests];
	bool old_debug = self->Debug;
	self->Debug = false;
	int i;
	for (i=0; i<n_tests; i++)
	{
		void *ptr = MemoryDriver_Allocate(self, 1+(i*i), __FILE__, __LINE__);
		if (!ptr)
		{
			printf("MemoryDriverTest: Error - ptr_%d is null after allocation\n",i);
			abort();
		}
		//printf("MemoryDriverTest: ptr_%d is set after allocation 0x%x\n",i,(uint32_t)ptr);
		pointers[i] = ptr;
	}

	for (i=0;i<n_tests; i++)
	{
		MemoryDriver_Free(self, (void*)pointers[i], __FILE__, __LINE__);

		if (MemoryDriver_IsValidBlock(self,pointers[i]))
		{
			printf("MemoryDriverTest: ptr_%d WAS NOT freed successfully\n",i);
			abort();
		}
	}
	self->Debug = old_debug;
	printf("MemoryDriverTest: Free tests passed\n");	
}

void MemoryDriverTest_RunSuite(struct MemoryDriver* self)
{
	MemoryDriverTest_Allocate(self);
	MemoryDriverTest_ReallocateCaseA(self);
	MemoryDriverTest_ReallocateCaseB(self);
	MemoryDriverTest_Free(self);
    MemoryDriver_PrintMemoryMap(self);
}