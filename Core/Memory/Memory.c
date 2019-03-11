#include "Memory.h"
#include "../Screen/Screen.h"
#include "../Utilities.h"

void tkMemory_Initialise()
{
    MemoryBlocksBegin = 0; 
    MemoryBaseAddress = 0;
    MemoryHeapEndAddress = 0;
    tkMemory_Detect();
}

uint32_t tkMemory_FindFreeBlock(uint32_t size)
{
    #ifdef __DEBUG_MEMORY
        static char addr[BUFLEN];
        tkScreen_PrintLine("Memory: Finding a free block");
    #endif

    if (MemoryBlocksBegin == 0)
    {
        MemoryBlocksBegin = MemoryBaseAddress;
        MemoryHeapEndAddress = MemoryBaseAddress;
        return MemoryBaseAddress;
    }

    tkMemory_BlockHeader* current = (tkMemory_BlockHeader*)MemoryBlocksBegin;
    while((uint32_t)current != MemoryHeapEndAddress)
    {
        if (current->mInUse) 
        {
            #ifdef __DEBUG_MEMORY
                memset(addr,0,BUFLEN);
                itoa((uint32_t)current,addr,BASE_16);
                tkScreen_Print("Memory: Current block in use 0x");
                tkScreen_PrintLine(addr);
            #endif

            if ((uint32_t)current + sizeof(tkMemory_BlockHeader) + current->mSize != MemoryHeapEndAddress)
            {
                current = (tkMemory_BlockHeader*)(
                    ((uint32_t)current) + 
                    sizeof(tkMemory_BlockHeader) + 
                    current->mSize);

                #ifdef __DEBUG_MEMORY
                    memset(addr,0,BUFLEN);
                    itoa((uint32_t)current,addr,BASE_16);
                    tkScreen_Print("Memory: Stepped to next 0x");
                    tkScreen_PrintLine(addr);
                #endif
            }
            // No next block, assign to end of heap
            else
            {
                current = (tkMemory_BlockHeader*)MemoryHeapEndAddress;
                #ifdef __DEBUG_MEMORY
                    memset(addr,0,BUFLEN);
                    itoa((uint32_t)current,addr,BASE_16);
                    tkScreen_Print("Memory: No free block found, using heap end 0x");
                    tkScreen_PrintLine(addr);
                #endif
                return (uint32_t)current;
            }
        }
        else if (current->mSize >= size)
        {
            #ifdef __DEBUG_MEMORY
                memset(addr,0,BUFLEN);
                itoa((uint32_t)current,addr,BASE_16);
                tkScreen_Print("Memory: Found unused block with enough space 0x");
                tkScreen_PrintLine(addr);
            #endif
            return (uint32_t)current;
        }
    }

    return (uint32_t)current;
}

tkMemory_BlockHeader* tkMemory_GetPreviousBlock(tkMemory_BlockHeader* block)
{
    tkMemory_BlockHeader* b = (tkMemory_BlockHeader*)MemoryBlocksBegin;
    while(b + sizeof(tkMemory_BlockHeader) + b->mSize != block)
    {
        b = (tkMemory_BlockHeader*)b+sizeof(tkMemory_BlockHeader)+b->mSize;
        if ((uint32_t)b > MemoryHeapEndAddress)
        {
            return 0;
        }
    }
    return b;
}

void* tkMemory_Allocate(uint32_t size)
{
    #ifdef __DEBUG_MEMORY
        static char addressBuffer[BUFLEN];
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
    #endif

    // Cannot allocate zero size block
    if (size == 0)
    {
        #ifdef __DEBUG_MEMORY
            tkScreen_PrintLine("Memory: Cannot allocate 0 size block");
        #endif
        return 0;
    }

    uint32_t freeBlock = tkMemory_FindFreeBlock(size);
    uint32_t totalRequestedSize = sizeof(tkMemory_BlockHeader) + size;

    tkMemory_BlockHeader* blockHeader = ((tkMemory_BlockHeader*)freeBlock);
    tkMemory_BlockHeader* afterBlockHeader = blockHeader+1;

    #ifdef __DEBUG_MEMORY
        itoa(freeBlock,addressBuffer,BASE_16);
        tkScreen_Print("Memory: Allocated Block HDR.0x"); tkScreen_Print(addressBuffer);
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
        itoa((uint32_t)afterBlockHeader,addressBuffer,BASE_16);
        tkScreen_Print(" DATA.0x"); tkScreen_Print(addressBuffer);
        itoa(freeBlock+totalRequestedSize,addressBuffer,BASE_16);
        tkScreen_Print(" END.0x"); tkScreen_PrintLine(addressBuffer);
    #endif

    if (freeBlock == MemoryHeapEndAddress)
    {
        blockHeader->mInUse = 1;
        blockHeader->mSize = size;
        tkMemory_MoveHeapEnd(totalRequestedSize);
    }
    return (void*)afterBlockHeader;
}

uint32_t tkMemory_MoveHeapEnd(int32_t delta)
{
    #ifdef __DEBUG_MEMORY
        static char lengthBuffer[BUFLEN];
        memset(lengthBuffer,0,sizeof(char)*BUFLEN);
        itoa(delta,lengthBuffer,BASE_10);

        static char addressBuffer[BUFLEN];
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
        itoa(MemoryHeapEndAddress,addressBuffer,BASE_16);

        tkScreen_Print("Memory: Moving Heap End by "); tkScreen_Print(lengthBuffer);
        tkScreen_Print("b from 0x"); tkScreen_Print(addressBuffer);
        tkScreen_Print(" to 0x");
    #endif

    MemoryHeapEndAddress += delta;

    #ifdef __DEBUG_MEMORY
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
        itoa(MemoryHeapEndAddress,addressBuffer,BASE_16);
        tkScreen_PrintLine(addressBuffer);
    #endif

    return MemoryHeapEndAddress;
}

void tkMemory_Free(void* addr)
{
    uint32_t block = (uint32_t)addr;
    tkMemory_BlockHeader* header = ((tkMemory_BlockHeader*)block)-1;
    #ifdef __DEBUG_MEMORY
        static char addressBuffer[BUFLEN];
        memset(addressBuffer,0,sizeof(char)*BUFLEN);
        itoa((uint32_t)header,addressBuffer,BASE_16);
        tkScreen_Print("Memory: Freeing Memory Block at 0x"); tkScreen_PrintLine(addressBuffer);
    #endif
    header->mInUse = 0;
    // Last in heap
    if ((uint32_t)MemoryHeapEndAddress == (uint32_t)header+sizeof(tkMemory_BlockHeader)+header->mSize)
    {
        // Set the previous block to mark the end of allocated memory
        uint32_t totalSize = sizeof(tkMemory_BlockHeader)+header->mSize;
        tkMemory_MoveHeapEnd(-totalSize);
    }
}

void tkMemory_Detect()
{
    if (!MemoryMultibootInfo)
    {
        #ifdef __DEBUG_MEMORY
            tkScreen_PrintLine("Memory: Multiboot Info Not Found!");
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
        itoa(MemoryMultibootInfo->mmap_addr,mmapAddrBuf,BASE_16);
        itoa(MemoryMultibootInfo->mmap_length,mmapLenBuf,BASE_16);
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

                tkScreen_Print("Base [H0x");tkScreen_Print(baseAddrHighBuffer); tkScreen_Print(" L0x");tkScreen_Print(baseAddrLowBuffer);
                tkScreen_Print("], Length [H0x");tkScreen_Print(lengthHighBuffer); tkScreen_Print(" L0x");tkScreen_Print(lengthLowBuffer);
                tkScreen_Print("], Type 0x");tkScreen_Print(typeBuffer);
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
        itoa((uint32_t)MemoryBaseAddress,buffer,BASE_16);
        tkScreen_Print("Memory: Heap starts at 0x");
        tkScreen_PrintLine(buffer);
        memset(buffer,0,sizeof(char)*BUFLEN);
        itoa(sizeof(tkMemory_BlockHeader),buffer,BASE_10);
        tkScreen_Print("Memory: MemoryBlockHeader header is ");
        tkScreen_Print(buffer);
        tkScreen_PrintLine("b");
    #endif
}

void tkMemory_SetMultibootInfo(multiboot_info_t* mbi)
{
    MemoryMultibootInfo = mbi;
}

tkMemory_BlockCount tkMemory_CountUsedBlocks()
{
    tkMemory_BlockCount c;
    c.mBlocksUsed = 0;
    c.mSizeInBytes = 0;
    tkMemory_BlockHeader* current = (tkMemory_BlockHeader*)MemoryBlocksBegin;
    do
    {
        if (current->mInUse)
        {
            c.mBlocksUsed++;
            c.mSizeInBytes += sizeof(tkMemory_BlockHeader)+current->mSize;
        }
        current = (tkMemory_BlockHeader*)current + sizeof(tkMemory_BlockHeader) + current->mSize;
    }
    while (current < (tkMemory_BlockHeader*)MemoryHeapEndAddress);
    return c;
}

uint32_t tkMemory_GetLastBlock()
{
    tkMemory_BlockHeader* last = tkMemory_GetPreviousBlock((tkMemory_BlockHeader*)MemoryHeapEndAddress);
    return (uint32_t)last;
}
