#include "Memory.h"
#include "../Screen/Screen.h"
#include "../Utilities.h"

void tkMemoryInitialise()
{
    MemoryBlocksBegin = 0; 
    MemoryBaseAddress = 0;
    MemoryHeapEndAddress = 0;
    tkMemoryDetect();
}

uint32_t tkMemoryFindFreeBlock(uint32_t size)
{
    #ifdef __DEBUG_MEMORY
        static char addr[BUFLEN];
        tkScreenPrintLine("Memory: Finding a free block");
    #endif

    if (MemoryBlocksBegin == 0)
    {
        MemoryBlocksBegin = MemoryBaseAddress;
        MemoryHeapEndAddress = MemoryBaseAddress;
        return MemoryBaseAddress;
    }

    tkMemoryBlockHeader* current = (tkMemoryBlockHeader*)MemoryBlocksBegin;
    while((uint32_t)current != MemoryHeapEndAddress)
    {
        if (current->mInUse) 
        {
            #ifdef __DEBUG_MEMORY
                memset(addr,0,BUFLEN);
                itoa((uint32_t)current,addr,BASE_16);
                tkScreenPrint("Memory: Current block in use 0x");
                tkScreenPrintLine(addr);
            #endif

            if ((uint32_t)current + sizeof(tkMemoryBlockHeader) + current->mSize != MemoryHeapEndAddress)
            {
                current = (tkMemoryBlockHeader*)(
                    ((uint32_t)current) + 
                    sizeof(tkMemoryBlockHeader) + 
                    current->mSize);

                #ifdef __DEBUG_MEMORY
                    memset(addr,0,BUFLEN);
                    itoa((uint32_t)current,addr,BASE_16);
                    tkScreenPrint("Memory: Stepped to next 0x");
                    tkScreenPrintLine(addr);
                #endif
            }
            // No next block, assign to end of heap
            else
            {
                current = (tkMemoryBlockHeader*)MemoryHeapEndAddress;
                #ifdef __DEBUG_MEMORY
                    memset(addr,0,BUFLEN);
                    itoa((uint32_t)current,addr,BASE_16);
                    tkScreenPrint("Memory: No free block found, using heap end 0x");
                    tkScreenPrintLine(addr);
                #endif
                return (uint32_t)current;
            }
        }
        else if (current->mSize >= size)
        {
            #ifdef __DEBUG_MEMORY
                memset(addr,0,BUFLEN);
                itoa((uint32_t)current,addr,BASE_16);
                tkScreenPrint("Memory: Found unused block with enough space 0x");
                tkScreenPrintLine(addr);
            #endif
            return (uint32_t)current;
        }
    }

    return (uint32_t)current;
}

tkMemoryBlockHeader* tkMemoryGetPreviousBlock(tkMemoryBlockHeader* block)
{
    tkMemoryBlockHeader* b = (tkMemoryBlockHeader*)MemoryBlocksBegin;
    while(b + sizeof(tkMemoryBlockHeader) + b->mSize != block)
    {
        b = (tkMemoryBlockHeader*)b+sizeof(tkMemoryBlockHeader)+b->mSize;
        if ((uint32_t)b > MemoryHeapEndAddress)
        {
            return 0;
        }
    }
    return b;
}

void* tkMemoryAllocate(uint32_t size)
{
    #ifdef __DEBUG_MEMORY
        static char addressBuffer[BUFLEN];
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
    #endif

    // Cannot allocate zero size block
    if (size == 0)
    {
        #ifdef __DEBUG_MEMORY
            tkScreenPrintLine("Memory: Cannot allocate 0 size block");
        #endif
        return 0;
    }

    uint32_t freeBlock = tkMemoryFindFreeBlock(size);
    uint32_t totalRequestedSize = sizeof(tkMemoryBlockHeader) + size;

    tkMemoryBlockHeader* blockHeader = ((tkMemoryBlockHeader*)freeBlock);
    tkMemoryBlockHeader* afterBlockHeader = blockHeader+1;

    #ifdef __DEBUG_MEMORY
        itoa(freeBlock,addressBuffer,BASE_16);
        tkScreenPrint("Memory: Allocated Block HDR.0x"); tkScreenPrint(addressBuffer);
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
        itoa((uint32_t)afterBlockHeader,addressBuffer,BASE_16);
        tkScreenPrint(" DATA.0x"); tkScreenPrint(addressBuffer);
        itoa(freeBlock+totalRequestedSize,addressBuffer,BASE_16);
        tkScreenPrint(" END.0x"); tkScreenPrintLine(addressBuffer);
    #endif

    if (freeBlock == MemoryHeapEndAddress)
    {
        blockHeader->mInUse = 1;
        blockHeader->mSize = size;
        tkMemoryMoveHeapEnd(totalRequestedSize);
    }
    return (void*)afterBlockHeader;
}

uint32_t tkMemoryMoveHeapEnd(int32_t delta)
{
    #ifdef __DEBUG_MEMORY
        static char lengthBuffer[BUFLEN];
        memset(lengthBuffer,0,sizeof(char)*BUFLEN);
        itoa(delta,lengthBuffer,BASE_10);

        static char addressBuffer[BUFLEN];
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
        itoa(MemoryHeapEndAddress,addressBuffer,BASE_16);

        tkScreenPrint("Memory: Moving Heap End by "); tkScreenPrint(lengthBuffer);
        tkScreenPrint("b from 0x"); tkScreenPrint(addressBuffer);
        tkScreenPrint(" to 0x");
    #endif

    MemoryHeapEndAddress += delta;

    #ifdef __DEBUG_MEMORY
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
        itoa(MemoryHeapEndAddress,addressBuffer,BASE_16);
        tkScreenPrintLine(addressBuffer);
    #endif

    return MemoryHeapEndAddress;
}

void tkMemoryFree(void* addr)
{
    uint32_t block = (uint32_t)addr;
    tkMemoryBlockHeader* header = ((tkMemoryBlockHeader*)block)-1;
    #ifdef __DEBUG_MEMORY
        static char addressBuffer[BUFLEN];
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
        itoa((uint32_t)header,addressBuffer,BASE_16);
        tkScreenPrint("Memory: Freeing Memory Block at 0x"); tkScreenPrintLine(addressBuffer);
    #endif
    header->mInUse = 0;
    // Last in heap
    if ((uint32_t)MemoryHeapEndAddress == (uint32_t)header+sizeof(tkMemoryBlockHeader)+header->mSize)
    {
        // Set the previous block to mark the end of allocated memory
        uint32_t totalSize = sizeof(tkMemoryBlockHeader)+header->mSize;
        tkMemoryMoveHeapEnd(-totalSize);
    }
}

void tkMemoryDetect()
{
    if (!MemoryMultibootInfo)
    {
            tkScreenPrintLine("Memory: Multiboot Info Not Found!");
        return;
    }

    struct multiboot_mmap_entry* mmap = 0;
    if (MemoryMultibootInfo->flags & MULTIBOOT_INFO_MEMORY)
    {
        uint32_t mem_lower = (uint32_t)MemoryMultibootInfo->mem_lower;
        uint32_t mem_upper = (uint32_t)MemoryMultibootInfo->mem_upper;
        char lowerBuf[BUFLEN];
        char upperBuf[BUFLEN];
        memset(lowerBuf,0,sizeof(char)*BUFLEN);
        memset(upperBuf,0,sizeof(char)*BUFLEN);
        itoa(mem_lower,lowerBuf,BASE_10);
        itoa(mem_upper,upperBuf,BASE_10);
        tkScreenPrint("Memory Lower: ");
        tkScreenPrint(lowerBuf);
        tkScreenPrint("Kb, Upper: ");
        tkScreenPrint(upperBuf);
        tkScreenPrint("Kb");
        tkScreenNewLine();
    }

    if (MemoryMultibootInfo->flags & MULTIBOOT_INFO_MEM_MAP)
    {
        char mmapAddrBuf[BUFLEN];
        char mmapLenBuf[BUFLEN];
        memset(mmapAddrBuf,0,sizeof(char)*BUFLEN);
        memset(mmapLenBuf,0,sizeof(char)*BUFLEN);
        itoa(MemoryMultibootInfo->mmap_addr,mmapAddrBuf,BASE_16);
        itoa(MemoryMultibootInfo->mmap_length,mmapLenBuf,BASE_16);
        tkScreenPrint("MMap Addr: 0x");
        tkScreenPrint(mmapAddrBuf);
        tkScreenPrint(" MMap Length: 0x");
        tkScreenPrint(mmapLenBuf);
        tkScreenNewLine();

        for
        (
            mmap = (struct multiboot_mmap_entry*)MemoryMultibootInfo->mmap_addr;
            (uint32_t)mmap < (MemoryMultibootInfo->mmap_addr + MemoryMultibootInfo->mmap_length);
            mmap = (struct multiboot_mmap_entry*)((uint32_t)mmap + mmap->size + sizeof(mmap->size))
        )
        {
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

            tkScreenPrint("Base [H0x");tkScreenPrint(baseAddrHighBuffer); tkScreenPrint(" L0x");tkScreenPrint(baseAddrLowBuffer);
            tkScreenPrint("], Length [H0x");tkScreenPrint(lengthHighBuffer); tkScreenPrint(" L0x");tkScreenPrint(lengthLowBuffer);
            tkScreenPrint("], Type 0x");tkScreenPrint(typeBuffer);

            if ((mmap->addr & 0xFFFFFFFF) == UPPER_RAM_BASE)
            {
                tkScreenPrint(" <-- HEAP");
                MemoryBaseAddress = UPPER_RAM_BASE+UPPER_RAM_OFFSET;
                MemoryUpperRamSize = mmap->len;
            }
            tkScreenNewLine();
        }
    }
    #ifdef __DEBUG_MEMORY
        static char buffer[BUFLEN];
        memset(buffer,0,sizeof(char)*BUFLEN);
        itoa((uint32_t)MemoryBaseAddress,buffer,BASE_16);
        tkScreenPrint("Memory: Heap starts at 0x");
        tkScreenPrintLine(buffer);
        memset(buffer,0,sizeof(char)*BUFLEN);
        itoa(sizeof(tkMemoryBlockHeader),buffer,BASE_10);
        tkScreenPrint("Memory: MemoryBlockHeader header is ");
        tkScreenPrint(buffer);
        tkScreenPrintLine("b");
    #endif
}

void tkMemorySetMultibootInfo(multiboot_info_t* mbi)
{
    MemoryMultibootInfo = mbi;
}

tkMemoryBlockCount tkMemoryCountUsedBlocks()
{
    tkMemoryBlockCount c;
    c.mBlocksUsed = 0;
    c.mSizeInBytes = 0;
    tkMemoryBlockHeader* current = (tkMemoryBlockHeader*)MemoryBlocksBegin;
    do
    {
        if (current->mInUse)
        {
            c.mBlocksUsed++;
            c.mSizeInBytes += sizeof(tkMemoryBlockHeader)+current->mSize;
        }
        current = (tkMemoryBlockHeader*)current + sizeof(tkMemoryBlockHeader) + current->mSize;
    }
    while (current < (tkMemoryBlockHeader*)MemoryHeapEndAddress);
    return c;
}

uint32_t tkMemoryGetLastBlock()
{
    tkMemoryBlockHeader* last = tkMemoryGetPreviousBlock((tkMemoryBlockHeader*)MemoryHeapEndAddress);
    return (uint32_t)last;
}
