#include "LinkedListItem.h"
#include "../Memory/Memory.h"
#include "../Screen/Screen.h"

tkLinkedListItem* tkLinkedListItemConstruct()
{
    tkScreenPrintLine("LinkedListItem: Constructing");
    tkLinkedListItem* item = (tkLinkedListItem*)tkMemoryAllocate(sizeof(tkLinkedListItem));
    item->mData = 0;
    item->mNext = 0;
    return item;
}

void tkLinkedListItemDestruct(tkLinkedListItem* item)
{
    tkScreenPrintLine("LinkedListItem: Destructing");
    item->mData = 0;
    item->mNext = 0;
    tkMemoryFree(item);
}