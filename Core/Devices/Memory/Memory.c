#include "Memory.h"

void Memory_Constructor(multiboot_info_t* mbi_header)
{
	MemoryMultibootInfo = mbi_header;
	MemoryBlocksBegin = 0;
	MemoryBaseAddress = 0;
	MemoryHeapEndAddress = 0;
	Memory_Detect();
}

uint32_t Memory_FindFreeBlock(uint32_t size)
{
	#ifdef __DEBUG_MEMORY
		printf("Memory: Finding a free block";
	#endif

	if (MemoryBlocksBegin == 0)
	{
		MemoryBlocksBegin = MemoryBaseAddress;
		MemoryHeapEndAddress = MemoryBaseAddress;
		return MemoryBaseAddress;
	}

	MemoryBlockHeader* current = (MemoryBlockHeader*)MemoryBlocksBegin;
	while((uint32_t)current != MemoryHeapEndAddress)
	{
		if (current->mInUse)
		{
			#ifdef __DEBUG_MEMORY
				cout <<"Memory: Current block in use 0x" << current << endl;
			#endif

			if ((uint32_t)current + sizeof(MemoryBlockHeader) + current->mSize != MemoryHeapEndAddress)
			{
				current = (MemoryBlockHeader*)(((uint32_t)current) + sizeof(MemoryBlockHeader) + current->mSize);

				#ifdef __DEBUG_MEMORY
					printf("Memory: Stepped to next 0x" << current << endl;
				#endif
			}
			// No next block, assign to end of heap
			else
			{
				current = (MemoryBlockHeader*)MemoryHeapEndAddress;
				#ifdef __DEBUG_MEMORY
					printf("Memory: No free block found, using heap end 0x" << current << endl;
				#endif
				return (uint32_t)current;
			}
		}
		else if (current->mSize >= size)
		{
			#ifdef __DEBUG_MEMORY
				printf("Memory: Found unused block with enough space 0x" << addr;
			#endif
			return (uint32_t)current;
		}
	}

	return (uint32_t)current;
}

MemoryBlockHeader* Memory_GetPreviousBlock(MemoryBlockHeader* block)
{
	MemoryBlockHeader* b = (MemoryBlockHeader*)MemoryBlocksBegin;
	while(b + sizeof(MemoryBlockHeader) + b->mSize != block)
	{
		b = (MemoryBlockHeader*)b+sizeof(MemoryBlockHeader)+b->mSize;
		if ((uint32_t)b > MemoryHeapEndAddress)
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
			printf("Memory: Cannot allocate 0 size block" << endl;
		#endif
		return 0;
	}

	uint32_t freeBlock = Memory_FindFreeBlock(size);
	uint32_t totalRequestedSize = sizeof(MemoryBlockHeader) + size;

	MemoryBlockHeader* blockHeader = ((MemoryBlockHeader*)freeBlock);
	MemoryBlockHeader* afterBlockHeader = blockHeader+1;

	#ifdef __DEBUG_MEMORY
		printf("Memory: Allocated Block HDR.0x" << freeBlock
			 << " DATA.0x" << afterBlockHeader
			 << " END.0x" << freeBlock+totalRequestedSize
			 << endl;
	#endif

	if (freeBlock == MemoryHeapEndAddress)
	{
		blockHeader->mInUse = 1;
		blockHeader->mSize = size;
		Memory_MoveHeapEnd(totalRequestedSize);
	}
	return (void*)afterBlockHeader;
}

uint32_t Memory_MoveHeapEnd(int32_t delta)
{
	#ifdef __DEBUG_MEMORY
		printf("Memory: Moving Heap End by " << delta;;
			 << "b from 0x" << mHeapEndAddress << " to 0x";
	#endif

	MemoryHeapEndAddress += delta;

	#ifdef __DEBUG_MEMORY
		printf(MemoryHeapEndAddress << endl;
	#endif

	return MemoryHeapEndAddress;
}

void Memory_Free(void* addr)
{
	uint32_t block = (uint32_t)addr;
	MemoryBlockHeader* header = ((MemoryBlockHeader*)block)-1;
	#ifdef __DEBUG_MEMORY
		printf("Memory: Freeing Memory Block at 0x" << header << endl;
	#endif
	header->mInUse = 0;
	// Last in heap
	if ((uint32_t)MemoryHeapEndAddress == (uint32_t)header+sizeof(MemoryBlockHeader)+header->mSize)
	{
		// Set the previous block to mark the end of allocated memory
		uint32_t totalSize = sizeof(MemoryBlockHeader)+header->mSize;
		Memory_MoveHeapEnd(-totalSize);
	}
}

void Memory_Detect()
{
	if (!MemoryMultibootInfo)
	{
		#ifdef __DEBUG_MEMORY
			printf("Memory: Multiboot Info Not Found!\n");
		#endif
		return;
	}

	struct multiboot_mmap_entry* mmap = 0;
	if (MemoryMultibootInfo->flags & MULTIBOOT_INFO_MEMORY)
	{
		uint32_t mem_lower = (uint32_t)MemoryMultibootInfo->mem_lower;
		uint32_t mem_upper = (uint32_t)MemoryMultibootInfo->mem_upper;
		#ifdef __DEBUG_MEMORY
			char lowerBuf[BUFLEN];
			char upperBuf[BUFLEN];
			memset(lowerBuf,0,sizeof(char)*BUFLEN);
			memset(upperBuf,0,sizeof(char)*BUFLEN);
			itoa(mem_lower,lowerBuf,BASE_10);
			itoa(mem_upper,upperBuf,BASE_10);
			tkScreen_Print("Memory Lower: ");
			tkScreen_Print(lowerBuf);
			tkScreen_Print("Kb, Upper: ");
			tkScreen_Print(upperBuf);
			tkScreen_Print("Kb");
			tkScreen_NewLine();
		#endif
	}

	if (MemoryMultibootInfo->flags & MULTIBOOT_INFO_MEM_MAP)
	{
		#ifdef __DEBUG_MEMORY
		char mmapAddrBuf[BUFLEN];
		char mmapLenBuf[BUFLEN];
		memset(mmapAddrBuf,0,sizeof(char)*BUFLEN);
		memset(mmapLenBuf,0,sizeof(char)*BUFLEN);
		itoa(mMultibootInfo->mmap_addr,mmapAddrBuf,BASE_16);
		itoa(mMultibootInfo->mmap_length,mmapLenBuf,BASE_16);
		tkScreen_Print("MMap Addr: 0x");
		tkScreen_Print(mmapAddrBuf);
		tkScreen_Print(" MMap Length: 0x");
		tkScreen_Print(mmapLenBuf);
		tkScreen_NewLine();
		#endif

		for
		(
			mmap = (struct multiboot_mmap_entry*)MemoryMultibootInfo->mmap_addr;
			(uint32_t)mmap < (MemoryMultibootInfo->mmap_addr + MemoryMultibootInfo->mmap_length);
			mmap = (struct multiboot_mmap_entry*)((uint32_t)mmap + mmap->size + sizeof(mmap->size))
		)
		{
			#ifdef __DEBUG_MEMORY
				char baseAddrHighBuffer[BUFLEN];
				char baseAddrLowBuffer[BUFLEN];
				char lengthHighBuffer[BUFLEN];
				char lengthLowBuffer[BUFLEN];
				char typeBuffer[BUFLEN];

				memset(baseAddrHighBuffer,0,sizeof(char)*BUFLEN);
				memset(baseAddrLowBuffer,0,sizeof(char)*BUFLEN);
				memset(lengthHighBuffer,0,sizeof(char)*BUFLEN);
				memset(lengthLowBuffer,0,sizeof(char)*BUFLEN);
				memset(typeBuffer,0,sizeof(char)*BUFLEN);

				itoa(mmap->addr >> 32, baseAddrHighBuffer, BASE_16);
				itoa(mmap->addr & 0xFFFFFFFF, baseAddrLowBuffer, BASE_16);
				itoa(mmap->len >> 32, lengthHighBuffer, BASE_16);
				itoa(mmap->len & 0xFFFFFFFF, lengthLowBuffer, BASE_16);
				itoa(mmap->type, typeBuffer, BASE_16);

				cout("Base [H0x");
				cout(baseAddrHighBuffer);
				cout(" L0x");
				cout(baseAddrLowBuffer);
				cout("], Length [H0x");
				cout(lengthHighBuffer);
				cout(" L0x");
				cout(lengthLowBuffer);
				cout("], Type 0x");
				cout(typeBuffer);

			#endif

			if ((mmap->addr & 0xFFFFFFFF) == UPPER_RAM_BASE)
			{
				#ifdef __DEBUG_MEMORY
				tkScreen_Print(" <-- HEAP");
				#endif
				MemoryBaseAddress = UPPER_RAM_BASE+UPPER_RAM_OFFSET;
				MemoryUpperRamSize = mmap->len;
			}
			#ifdef __DEBUG_MEMORY
				tkScreen_NewLine();
			#endif
		}
	}
	#ifdef __DEBUG_MEMORY
		static char buffer[BUFLEN];
		memset(buffer,0,sizeof(char)*BUFLEN);
		itoa((uint32_t)mBaseAddress,buffer,BASE_16);
		tkScreen_Print("Memory: Heap starts at 0x");
		tkScreen_PrintLine(buffer);
		memset(buffer,0,sizeof(char)*BUFLEN);
		itoa(sizeof(MemoryBlockHeader),buffer,BASE_10);
		tkScreen_Print("Memory: MemoryBlockHeader header is ");
		tkScreen_Print(buffer);
		tkScreen_PrintLine("b");
	#endif
}

void Memory_SetMultibootInfo(multiboot_info_t* mbi)
{
	MemoryMultibootInfo = mbi;
}

MemoryBlockCount Memory_CountUsedBlocks()
{
	MemoryBlockCount c;
	c.mBlocksUsed = 0;
	c.mSizeInBytes = 0;
	MemoryBlockHeader* current = (MemoryBlockHeader*)MemoryBlocksBegin;
	do
	{
		if (current->mInUse)
		{
			c.mBlocksUsed++;
			c.mSizeInBytes += sizeof(MemoryBlockHeader)+current->mSize;
		}
		current = (MemoryBlockHeader*)current + sizeof(MemoryBlockHeader) + current->mSize;
	}
	while (current < (MemoryBlockHeader*)MemoryHeapEndAddress);
	return c;
}

uint32_t Memory_GetLastBlock()
{
	MemoryBlockHeader* last = Memory_GetPreviousBlock((MemoryBlockHeader*)MemoryHeapEndAddress);
	return (uint32_t)last;
}
