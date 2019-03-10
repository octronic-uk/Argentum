#pragma once
#include "../Types.h"
#include "LinkedListItem.h"

typedef struct tkLinkedList
{
    tkLinkedListItem* mBegin;
} tkLinkedList;

tkLinkedList* tkLinkedListConstruct();
void tkLinkedListDestruct(tkLinkedList* list);

tkLinkedListItem* tkLinkedListItemConstruct();
void tkLinkedListItemDestruct(tkLinkedListItem* item);

tkLinkedListItem* tkLinkedListAt(tkLinkedList* list, uint32_t index);
tkLinkedListItem** tkLinkedListGetLast(tkLinkedList* list);
tkLinkedListItem* tkLinkedListFind(tkLinkedList* list, void* data);

uint32_t tkLinkedListSize(tkLinkedList* list);

tkLinkedListItem* tkLinkedListInsert(tkLinkedList* list, void* data);
void tkLinkedListRemove(tkLinkedList* list, tkLinkedListItem* item);
void tkLinkedListFreeAllData(tkLinkedList* list);
