#include "LinkedList.h"

#include "../../Core/Types.h"
#include "../../Core/Utilities.h"
#include "../../Core/Memory/Memory.h"

#ifdef __DEBUG_LINKED_LIST
    #include "../../Core/Screen/Screen.h"
#endif

tkLinkedList* tkLinkedList_Construct()
{
    #ifdef __DEBUG_LINKED_LIST
        tkScreen_PrintLine("LinkedList: Constructing");
    #endif
    tkLinkedList* list = (tkLinkedList*)tkMemory_Allocate(sizeof(tkLinkedList));
    list->mBegin = 0;
    return list;
}

void tkLinkedList_Destruct(tkLinkedList* list)
{
    #ifdef __DEBUG_LINKED_LIST
        tkScreen_PrintLine("LinkedList: Destructing");
    #endif
    tkLinkedListItem* item = list->mBegin;
    while (item)
    {
        tkLinkedListItem* next = item->mNext;
        tkLinkedListItem_Destruct(item);
        item = next;
    }
    tkMemory_Free(list);
}

tkLinkedListItem** tkLinkedList_GetLast(tkLinkedList* list)
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
    #ifdef __DEBUG_LINKED_LIST
        tkScreen_PrintLine("LinkedList: Error getting last item");
    #endif
    return 0;
}

tkLinkedListItem* tkLinkedList_Find(tkLinkedList* list, void* data)
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

tkLinkedListItem* tkLinkedList_Insert(tkLinkedList* list, void* data)
{
    tkLinkedListItem** last = tkLinkedList_GetLast(list);
    (*last) = (tkLinkedListItem*)tkLinkedListItem_Construct();
    (*last)->mData = data;

    #ifdef __DEBUG_LINKED_LIST
        static char buf[BUFLEN];
        memset(buf,0,BUFLEN);
        itoa((uint32_t)data,buf,BASE_16);
        tkScreen_Print("LinkedList: Inserting data 0x"); 
        tkScreen_Print(buf); 
        tkScreen_Print(" into item at 0x");
        memset(buf,0,BUFLEN);
        itoa((uint32_t)(*last),buf,BASE_16);
        tkScreen_PrintLine(buf);
    #endif
    return *last;
}


void tkLinkedList_Remove(tkLinkedList* list, tkLinkedListItem* item)
{

}

uint32_t tkLinkedList_Size(tkLinkedList* list)
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

tkLinkedListItem* tkLinkedList_At(tkLinkedList* list, uint32_t index)
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

void tkLinkedList_FreeAllData(tkLinkedList* list)
{
    tkLinkedListItem* item = list->mBegin;
    while (item)
    {
        if (item->mData)
        {
            tkMemory_Free(item->mData);
        }
        item=item->mNext;
    }
}
