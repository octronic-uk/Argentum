#include "LinkedList.h"

#include "../Types.h"
#include "../Utilities.h"
#include "../Screen/Screen.h"
#include "../Memory/Memory.h"

tkLinkedList* tkLinkedListConstruct()
{
    tkScreenPrintLine("LinkedList: Constructing");
    tkLinkedList* list = (tkLinkedList*)tkMemoryAllocate(sizeof(tkLinkedList));
    list->mBegin = 0;
    return list;
}

void tkLinkedListDestruct(tkLinkedList* list)
{
    tkScreenPrintLine("LinkedList: Destructing");
    tkLinkedListItem* item = list->mBegin;
    while (item)
    {
        tkLinkedListItem* next = item->mNext;
        tkLinkedListItemDestruct(item);
        item = next;
    }
    tkMemoryFree(list);
}

tkLinkedListItem** tkLinkedListGetLast(tkLinkedList* list)
{
    if (list->mBegin == 0)
    {
        return &(list->mBegin);
    }

    tkLinkedListItem* item = list->mBegin;
    while(item)
    {
        if (item->mNext == 0)
        {
            return &(item->mNext);
        }
        item = item->mNext;
    }
    tkScreenPrintLine("LinkedList: Error getting last item");
    return 0;
}

tkLinkedListItem* tkLinkedListFind(tkLinkedList* list, void* data)
{
    tkLinkedListItem* item = list->mBegin;
    while (item)
    {
        if (item->mData == data)
        {
            return item;
        }
        item = item->mNext;
    }
    return 0;
}

tkLinkedListItem* tkLinkedListInsert(tkLinkedList* list, void* data)
{
    tkLinkedListItem** last = tkLinkedListGetLast(list);
    (*last) = (tkLinkedListItem*)tkLinkedListItemConstruct();
    (*last)->mData = data;

    static char buf[BUFLEN];
    memset(buf,0,BUFLEN);
    itoa((uint32_t)data,buf,BASE_16);
    tkScreenPrint("LinkedList: Inserting data 0x"); tkScreenPrint(buf); tkScreenPrint(" into item at 0x");
    memset(buf,0,BUFLEN);
    itoa((uint32_t)(*last),buf,BASE_16);
    tkScreenPrintLine(buf);
    return *last;
}


void tkLinkedListRemove(tkLinkedList* list, tkLinkedListItem* item)
{

}

uint32_t tkLinkedListSize(tkLinkedList* list)
{
    uint32_t count = 0;
    tkLinkedListItem* item = list->mBegin;
    while (item)
    {
        item = item->mNext;
        count++;
    }
    return count;
}

tkLinkedListItem* tkLinkedListAt(tkLinkedList* list, uint32_t index)
{
    uint32_t count = 0;
    tkLinkedListItem* item = list->mBegin;
    while (count < index)
    {
        item = item->mNext;
        count++;
    }
    return item;
}

void tkLinkedListFreeAllData(tkLinkedList* list)
{
    tkLinkedListItem* item = list->mBegin;
    while (item)
    {
        if (item->mData)
        {
            tkMemoryFree(item->mData);
        }
        item=item->mNext;
    }
}