#pragma once

typedef struct tkLinkedListItem
{
    void* mData;
    struct tkLinkedListItem* mNext;
} tkLinkedListItem;