#pragma once
#include "../../Core/Types.h"
#include "LinkedListItem.h"

typedef struct tkLinkedList
{
    tkLinkedListItem* mBegin;
} tkLinkedList;

tkLinkedList* tkLinkedList_Construct();
void tkLinkedList_Destruct(tkLinkedList* list);

tkLinkedListItem* tkLinkedList_At(tkLinkedList* list, uint32_t index);
tkLinkedListItem** tkLinkedList_GetLast(tkLinkedList* list);
tkLinkedListItem* tkLinkedList_Find(tkLinkedList* list, void* data);
uint32_t tkLinkedList_Size(tkLinkedList* list);
tkLinkedListItem* tkLinkedList_Insert(tkLinkedList* list, void* data);
void tkLinkedList_Remove(tkLinkedList* list, tkLinkedListItem* item);
void tkLinkedList_FreeAllData(tkLinkedList* list);
