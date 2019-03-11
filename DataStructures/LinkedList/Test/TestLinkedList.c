#include "TestLinkedList.h"
#include "../../../Core/Screen/Screen.h"

void testLinkedList_Construct()
{
    tkScreen_NewLine();
    tkScreen_PrintLine("*** Testing LinkedList Construct ***");

    tkLinkedList* list = tkLinkedList_Construct();

    if (list)
    {
        tkScreen_PrintLine("Successfully Created List");
    }
    else
    {
        tkScreen_PrintLine("Error creating list in memory");
        return;
    }

    if (list->mBegin == 0)
    {
        tkScreen_PrintLine("Successfully init'd list->mBegin");
    }
    else
    {
        tkScreen_PrintLine("Error list->mItemList not null");
        return;
    }
    tkLinkedList_Destruct(list);
    tkScreen_NewLine();
    return;
}

void testLinkedList_Destruct()
{
    tkScreen_NewLine();
    tkScreen_PrintLine("*** Testing LinkedList Destruct ***");

    tkLinkedList* list  = tkLinkedList_Construct();
    tkLinkedList_Destruct(list);

    if (list->mBegin == 0)
    {
        tkScreen_PrintLine("Successfuly destroyed LinkedList");
    }
    else
    {
        tkScreen_PrintLine("Error destroying LinkedList");
    }
    tkScreen_NewLine();
    return;
}

void testLinkedList_Insert()
{
    tkScreen_NewLine();
    tkScreen_PrintLine("*** Testing LinkedListInsert ***");
    tkLinkedList* list = (tkLinkedList*)tkLinkedList_Construct();
    char some_data[] =  {'S','o','m','e',' ','D','a','t','a',0};
    tkLinkedListItem* inserted = tkLinkedList_Insert(list,(void*)some_data);
    if (inserted)
    {
        tkScreen_PrintLine("\tSuccessfully inserted item");
    }
    else
    {
        tkScreen_PrintLine("\tSuccessfully inserted item");
    }
    tkLinkedList_Destruct(list);
    tkScreen_NewLine();
    
}

void testLinkedList_Remove()
{
    tkScreen_NewLine();
    tkScreen_PrintLine("*** Testing LinkedListCount ***");
    tkLinkedList* list = (tkLinkedList*)tkLinkedList_Construct();
    char some_data[] =  {'S','o','m','e',' ','D','a','t','a',0};
    tkLinkedListItem* inserted = tkLinkedList_Insert(list,(void*)some_data);

    if (inserted)
    {
        tkScreen_PrintLine("\tSuccessfully inserted item");
    }
    else
    {
        tkScreen_PrintLine("\tSuccessfully inserted item");
        return;
    }

    if (tkLinkedList_Size(list) == 1)
    {
        tkScreen_PrintLine("\tSuccessfully got size");
    }
    else
    {
        tkScreen_PrintLine("Error getting size");
        return;
    }

    tkLinkedList_Remove(list,inserted);

    if (tkLinkedList_Size(list) == 1)
    {
        tkScreen_PrintLine("\tSuccessfully removed item");
    }
    else
    {
        tkScreen_PrintLine("Error removing item");
        return;
    }
    
    tkLinkedList_Destruct(list);
    tkScreen_NewLine();
}

void testLinkedList_Count()
{
    tkScreen_NewLine();
    tkScreen_PrintLine("*** Testing LinkedListCount ***");
    tkLinkedList* list = (tkLinkedList*)tkLinkedList_Construct();
    char some_data[] =  {'S','o','m','e',' ','D','a','t','a',0};
    tkLinkedListItem* inserted = tkLinkedList_Insert(list,(void*)some_data);

    if (inserted)
    {
        tkScreen_PrintLine("\tSuccessfully inserted item");
    }
    else
    {
        tkScreen_PrintLine("\tSuccessfully inserted item");
        return;
    }

    if (tkLinkedList_Size(list) == 1)
    {
        tkScreen_PrintLine("\tSuccessfully got size");
    }
    else
    {
        tkScreen_PrintLine("Error getting size");
        return;
    }
    
    tkLinkedList_Destruct(list);
    tkScreen_NewLine();
}

void testLinkedList_At()
{

}

void testLinkedList_GetLast()
{

}
void testLinkedList_Find()
{

}

void testLinkedList_Size()
{

}

void testLinkedList_FreeAllData()
{

}