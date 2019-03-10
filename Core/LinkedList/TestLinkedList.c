#include "TestLinkedList.h"
#include "../Screen/Screen.h"

void testLinkedListConstruct()
{
    tkScreenNewLine();
    tkScreenPrintLine("*** Testing LinkedList Construct ***");

    tkLinkedList* list = tkLinkedListConstruct();

    if (list)
    {
        tkScreenPrintLine("Successfully Created List");
    }
    else
    {
        tkScreenPrintLine("Error creating list in memory");
        return;
    }

    if (list->mBegin == 0)
    {
        tkScreenPrintLine("Successfully init'd list->mBegin");
    }
    else
    {
        tkScreenPrintLine("Error list->mItemList not null");
        return;
    }
    tkLinkedListDestruct(list);
    tkScreenNewLine();
    return;
}

void testLinkedListDestruct()
{
    tkScreenNewLine();
    tkScreenPrintLine("*** Testing LinkedList Destruct ***");

    tkLinkedList* list  = tkLinkedListConstruct();
    tkLinkedListDestruct(list);

    if (list->mBegin == 0)
    {
        tkScreenPrintLine("Successfuly destroyed LinkedList");
    }
    else
    {
        tkScreenPrintLine("Error destroying LinkedList");
    }
    tkScreenNewLine();
    return;
}

void testLinkedListInsert()
{
    tkScreenNewLine();
    tkScreenPrintLine("*** Testing LinkedListInsert ***");
    tkLinkedList* list = (tkLinkedList*)tkLinkedListConstruct();
    char some_data[] =  {'S','o','m','e',' ','D','a','t','a',0};
    tkLinkedListItem* inserted = tkLinkedListInsert(list,(void*)some_data);
    if (inserted)
    {
        tkScreenPrintLine("\tSuccessfully inserted item");
    }
    else
    {
        tkScreenPrintLine("\tSuccessfully inserted item");
    }
    tkLinkedListDestruct(list);
    tkScreenNewLine();
    
}

void testLinkedListRemove()
{
    tkScreenNewLine();
    tkScreenPrintLine("*** Testing LinkedListCount ***");
    tkLinkedList* list = (tkLinkedList*)tkLinkedListConstruct();
    char some_data[] =  {'S','o','m','e',' ','D','a','t','a',0};
    tkLinkedListItem* inserted = tkLinkedListInsert(list,(void*)some_data);

    if (inserted)
    {
        tkScreenPrintLine("\tSuccessfully inserted item");
    }
    else
    {
        tkScreenPrintLine("\tSuccessfully inserted item");
        return;
    }

    if (tkLinkedListSize(list) == 1)
    {
        tkScreenPrintLine("\tSuccessfully got size");
    }
    else
    {
        tkScreenPrintLine("Error getting size");
        return;
    }

    tkLinkedListRemove(list,inserted);

    if (tkLinkedListSize(list) == 1)
    {
        tkScreenPrintLine("\tSuccessfully removed item");
    }
    else
    {
        tkScreenPrintLine("Error removing item");
        return;
    }
    
    tkLinkedListDestruct(list);
    tkScreenNewLine();
}

void testLinkedListCount()
{
    tkScreenNewLine();
    tkScreenPrintLine("*** Testing LinkedListCount ***");
    tkLinkedList* list = (tkLinkedList*)tkLinkedListConstruct();
    char some_data[] =  {'S','o','m','e',' ','D','a','t','a',0};
    tkLinkedListItem* inserted = tkLinkedListInsert(list,(void*)some_data);

    if (inserted)
    {
        tkScreenPrintLine("\tSuccessfully inserted item");
    }
    else
    {
        tkScreenPrintLine("\tSuccessfully inserted item");
        return;
    }

    if (tkLinkedListSize(list) == 1)
    {
        tkScreenPrintLine("\tSuccessfully got size");
    }
    else
    {
        tkScreenPrintLine("Error getting size");
        return;
    }
    
    tkLinkedListDestruct(list);
    tkScreenNewLine();
}