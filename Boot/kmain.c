#include "../Kernel.h"
#include "../Core/Keyboard/Keyboard.h"
#include "../Core/Screen/Screen.h"
#include "../Core/Utilities.h"
#include "../Core/Memory/Memory.h"

#include "../Core/LinkedList/TestLinkedList.h"
#include "../Core/LinkedList/TestLinkedListItem.h"

void kmain(multiboot_info_t* mbi)
{
    tkMemorySetMultibootInfo(mbi);
    tkScreenInit();
    tkKeyboardInit();
    tkScreenPrintLine("Taskie Version 1");

    tkKernel k;
    tkKernelInit(&k);

    // Test LinkedList Structures
    tkKernelCreateTask(&k, "Test LinkedList Construct" , testLinkedListConstruct);
    tkKernelCreateTask(&k, "Test LinkedList Destruct" , testLinkedListDestruct);
    tkKernelCreateTask(&k, "Test LinkedList Insert" , testLinkedListInsert);
    tkKernelCreateTask(&k, "Test LinkedList Remove" , testLinkedListRemove);
    tkKernelCreateTask(&k, "Test LinkedList Count" , testLinkedListCount);

    tkKernelCreateTask(&k, "Test LinkedListItem Construct" , testLinkedListItemConstruct);
    tkKernelCreateTask(&k, "Test LinkedListItem Destruct" , testLinkedListItemDestruct);

    tkKernelExecute(&k);
    tkKernelDestruct(&k);

    tkMemoryBlockCount count = tkMemoryCountUsedBlocks();

    static char countBuf[BUFLEN];
    static char sizeBuf[BUFLEN];

    memset(countBuf,0,BUFLEN);
    memset(sizeBuf,0,BUFLEN);

    itoa(count.mBlocksUsed,countBuf,BASE_10);
    itoa(count.mSizeInBytes,sizeBuf,BASE_10);

    tkScreenPrint(countBuf); 
    tkScreenPrint(" blocks in use ("); 
    tkScreenPrint(sizeBuf); 
    tkScreenPrintLine(" bytes)");

    tkScreenPrintLine("Taskie is Doneskie!");

    // spin
    while(1);
    asm("cli");
    asm("hlt");
}

