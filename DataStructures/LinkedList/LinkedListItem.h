#pragma once

typedef struct tkLinkedListItem
{
    void* mData;
    struct tkLinkedListItem* mNext;
} tkLinkedListItem;

tkLinkedListItem* tkLinkedListItem_Construct();
void tkLinkedListItem_Destruct(tkLinkedListItem* item);