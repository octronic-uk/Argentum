#include "TestLinkedListItem.h"
#include "../../../Core/Screen/Screen.h"

void testLinkedListItem_Construct()
{
    tkScreen_NewLine();
    tkScreen_PrintLine("*** Testing LinkedListItem Construct ***");
    tkLinkedListItem* item = tkLinkedListItem_Construct();

    tkScreen_PrintLine("\tConstructed");

    if (item->mData == 0)
    {
        tkScreen_PrintLine("\tSuccessfuly init'd data");
    }
    else
    {
        tkScreen_PrintLine("\tError data is not null");
        return;
    }
    
    if (item->mNext == 0)
    {
        tkScreen_PrintLine("\tSuccessfuly init'd next");
    }
    else
    {
        tkScreen_PrintLine("\tError next is not null");
        return;
    }
    tkLinkedListItem_Destruct(item);
    tkScreen_NewLine();
    return;
}

void testLinkedListItem_Destruct()
{
    tkScreen_NewLine();
    tkScreen_PrintLine("*** Testing LinkedListItem Destruct ***");
    tkLinkedListItem* item = tkLinkedListItem_Construct();
    tkLinkedListItem_Destruct(item);

    if (item->mData == 0)
    {
        tkScreen_PrintLine("\tSuccessfuly destroyed data");
    }
    else
    {
        tkScreen_PrintLine("\tError data is not null");
        return;
    }
    
    if (item->mNext == 0)
    {
        tkScreen_PrintLine("\tSuccessfuly destroyed next");
    }
    else
    {
        tkScreen_PrintLine("\tError next is not null");
        return;
    }
    tkScreen_NewLine();
    return;
}
