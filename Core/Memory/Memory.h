#pragma once

struct tkMemoryBlockHeaderStruct
{
    unsigned long mSize;
    char mInUse;
    struct tkMemoryBlockHeaderStruct* mPrevious;
    struct tkMemoryBlockHeaderStruct* mNext;

} __attribute__((packed));

typedef struct tkMemoryBlockHeaderStruct tkMemoryBlockHeader;

static unsigned long MemoryBaseAddress;
static unsigned long MemoryHeapEndAddress;
static tkMemoryBlockHeader* MemoryBegin;

void tkMemoryInitialise();
int tkMemoryDetect();
tkMemoryBlockHeader* tkMemoryFindFreeBlock(unsigned long size);
void* tkMemoryAllocate(unsigned long size);
void tkMemoryFree(void* block);
unsigned long tkMemoryMoveHeapEnd(long delta);
