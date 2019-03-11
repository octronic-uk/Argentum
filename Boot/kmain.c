#include "../Kernel.h"
#include "../Core/Keyboard/Keyboard.h"
#include "../Core/Screen/Screen.h"
#include "../Core/Utilities.h"
#include "../Core/Memory/Memory.h"

#ifdef __UNIT_TESTS
    #include "../DataStructures/LinkedList/Test/TestLinkedList.h"
    #include "../DataStructures/LinkedList/Test/TestLinkedListItem.h"
#endif

void kmain(multiboot_info_t* mbi)
{
    tkMemory_SetMultibootInfo(mbi);
    tkScreen_Init();
    tkKeyboard_Init();
    tkScreen_PrintLine("Taskie Version 1");

    tkKernel k;
    tkKernel_Init(&k);

    #ifdef __UNIT_TESTS
        // Test LinkedList Structures
        tkKernel_CreateTask(&k, "Test LinkedList Construct" , testLinkedList_Construct);
        tkKernel_CreateTask(&k, "Test LinkedList Destruct" , testLinkedList_Destruct);
        tkKernel_CreateTask(&k, "Test LinkedList At" , testLinkedList_At);
        tkKernel_CreateTask(&k, "Test LinkedList GetLast" , testLinkedList_GetLast);
        tkKernel_CreateTask(&k, "Test LinkedList Find" , testLinkedList_Find);
        tkKernel_CreateTask(&k, "Test LinkedList Size" , testLinkedList_Size);
        tkKernel_CreateTask(&k, "Test LinkedList Insert" , testLinkedList_Insert);
        tkKernel_CreateTask(&k, "Test LinkedList Remove" , testLinkedList_Remove);
        tkKernel_CreateTask(&k, "Test LinkedList FreeAllData" , testLinkedList_FreeAllData);

        tkKernel_CreateTask(&k, "Test LinkedListItem Construct" , testLinkedListItem_Construct);
        tkKernel_CreateTask(&k, "Test LinkedListItem Destruct" , testLinkedListItem_Destruct);
    #endif

    tkKernel_Execute(&k);
    tkKernel_Destruct(&k);

    tkMemory_BlockCount count = tkMemory_CountUsedBlocks();

    static char countBuf[BUFLEN];
    static char sizeBuf[BUFLEN];

    memset(countBuf,0,BUFLEN);
    memset(sizeBuf,0,BUFLEN);

    itoa(count.mBlocksUsed,countBuf,BASE_10);
    itoa(count.mSizeInBytes,sizeBuf,BASE_10);

    tkScreen_Print(countBuf); 
    tkScreen_Print(" blocks in use ("); 
    tkScreen_Print(sizeBuf); 
    tkScreen_PrintLine(" bytes)");
    tkScreen_PrintLine("Taskie is Doneskie!");
    // spin
    while(1);
    asm("cli");
    asm("hlt");
}

