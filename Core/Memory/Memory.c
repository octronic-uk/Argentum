#include "Memory.h"
#include "../Screen/Screen.h"
#include "../Utilities.h"

void tkMemoryInitialise()
{
    tkMemoryDetect();
    MemoryBegin = (tkMemoryBlockHeader*)MemoryBaseAddress;
    MemoryBegin->mSize  = 0;
    MemoryBegin->mInUse = 1;
    MemoryBegin->mPrevious = MemoryBegin;
    MemoryBegin->mNext  = MemoryBegin;
}

tkMemoryBlockHeader* tkMemoryFindFreeBlock(unsigned long size)
{
    tkMemoryBlockHeader* current = MemoryBegin;

    // Check existing blocks

    while (current != current->mNext);
    {
       if (!current->mInUse && current->mSize >= size) 
       {
           return current;
       }
       else
       {
           current = current->mNext;
       }
    }

    return 0;
}

void* tkMemoryAllocate(unsigned long size)
{
    if (size == 0)
    {
        return 0;
    }

    tkMemoryBlockHeader* block = tkMemoryFindFreeBlock(size);
    if (block)
    {
        block->mInUse = 1;
        // Return address to the memory AFTER the header
        tkMemoryBlockHeader* afterBlock = &block[1]; 
        return (void*)afterBlock;
    }

    // No suitable memory block found

    unsigned long totalRequestedSize = sizeof(tkMemoryBlockHeader) + size; 

    unsigned long newBlockAddress = tkMemoryMoveHeapEnd(totalRequestedSize);

    tkMemoryBlockHeader* newBlock = (tkMemoryBlockHeader*)newBlockAddress;
    newBlock->mInUse = 1;
    newBlock->mSize = size;
    newBlock->mNext = 0;
    newBlock+1;
}

unsigned long tkMemoryMoveHeapEnd(long delta)
{
    static char buffer[BUFLEN];
    memset(buffer,0,sizeof(char)*BUFLEN);
    ultoa(MemoryHeapEndAddress,buffer,BASE_16);
    tkScreenPrint("Moving Heap End from 0x");
    tkScreenPrint(buffer);
    tkScreenPrint(" to 0x");
    MemoryHeapEndAddress += delta;
    memset(buffer,0,sizeof(char)*BUFLEN);
    ultoa(MemoryHeapEndAddress,buffer,BASE_16);
    tkScreenPrintLine(buffer);
}

void tkMemoryFree(void* block)
{
    tkMemoryBlockHeader* header = (tkMemoryBlockHeader*)(block-1);
    header->mInUse = 0;
    if (header->mNext == header->mNext)
    {
        // Set the previous block to mark the end of allocated memory
        header->mPrevious->mNext = 0;
        unsigned long totalSize = sizeof(tkMemoryBlockHeader)+header->mSize;
        tkMemoryMoveHeapEnd(-totalSize);
    }
}

int tkMemoryDetect()
{
    MemoryBaseAddress = 0x400000;
    MemoryHeapEndAddress = MemoryBaseAddress+sizeof(tkMemoryBlockHeader);
    static char buffer[BUFLEN];
    memset(buffer,0,sizeof(char)*BUFLEN);
    ultoa(MemoryBaseAddress,buffer,BASE_16);
    tkScreenPrint("Heap is located at 0x");
    tkScreenPrintLine(buffer);
}