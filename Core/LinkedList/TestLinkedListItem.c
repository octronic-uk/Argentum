#include "TestLinkedListItem.h"
#include "../Screen/Screen.h"

void testLinkedListItemConstruct()
{
    tkScreenNewLine();
    tkScreenPrintLine("*** Testing LinkedListItem Construct ***");
    tkLinkedListItem* item = tkLinkedListItemConstruct();

    tkScreenPrintLine("\tConstructed");

    if (item->mData == 0)
    {
        tkScreenPrintLine("\tSuccessfuly init'd data");
    }
    else
    {
        tkScreenPrintLine("\tError data is not null");
        return;
    }
    
    if (item->mNext == 0)
    {
        tkScreenPrintLine("\tSuccessfuly init'd next");
    }
    else
    {
        tkScreenPrintLine("\tError next is not null");
        return;
    }
    tkLinkedListItemDestruct(item);
    tkScreenNewLine();
    return;
}

void testLinkedListItemDestruct()
{
    tkScreenNewLine();
    tkScreenPrintLine("*** Testing LinkedListItem Destruct ***");
    tkLinkedListItem* item = tkLinkedListItemConstruct();
    tkLinkedListItemDestruct(item);

    if (item->mData == 0)
    {
        tkScreenPrintLine("\tSuccessfuly destroyed data");
    }
    else
    {
        tkScreenPrintLine("\tError data is not null");
        return;
    }
    
    if (item->mNext == 0)
    {
        tkScreenPrintLine("\tSuccessfuly destroyed next");
    }
    else
    {
        tkScreenPrintLine("\tError next is not null");
        return;
    }
    tkScreenNewLine();
    return;
}
