#include "LinkedListItem.h"
#include "../../Core/Memory/Memory.h"

#ifdef __DEBUG_LINKED_LIST
    #include "../../Core/Screen/Screen.h"
#endif

tkLinkedListItem* tkLinkedListItem_Construct()
{
    #ifdef __DEBUG_LINKED_LIST
        tkScreen_PrintLine("LinkedListItem: Constructing");
    #endif
    tkLinkedListItem* item = (tkLinkedListItem*)tkMemory_Allocate(sizeof(tkLinkedListItem));
    item->mData = 0;
    item->mNext = 0;
    return item;
}

void tkLinkedListItem_Destruct(tkLinkedListItem* item)
{
    #ifdef __DEBUG_LINKED_LIST
        tkScreen_PrintLine("LinkedListItem: Destructing");
    #endif
    item->mData = 0;
    item->mNext = 0;
    tkMemory_Free(item);
}