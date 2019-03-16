#include "LinkedList.h"
#include <Devices/Memory/Memory.h>
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

LinkedList* LinkedList_Constructor()
{
	LinkedList* list = Memory_Allocate(sizeof(LinkedList));
	if (!list)
	{
		return 0;
	}
	list->head = 0;
	return list;
}

void LinkedList_Destructor(LinkedList * list)
{
	LinkedListNode* current = list->head;
	LinkedListNode* next = current;
	while(current != 0)
	{
		next = current->next;
		Memory_Free(current);
		current = next;
	}
	Memory_Free(list);
}

void LinkedList_Display(LinkedList * list)
{
	LinkedListNode * current = list->head;
	if(list->head == 0)
	{
		return;
	}

	for(; current != 0; current = current->next)
	{
		printf("%d\n", (uint32_t)current->data);
	}
}

void LinkedList_PushBack(LinkedList* list, void* data)
{
	LinkedListNode * current = 0;
	if(list->head == 0)
	{
		list->head = LinkedList_CreateNode(data);
	}
	else
	{
		current = list->head;
		while (current->next!=0)
		{
			current = current->next;
		}
		current->next = LinkedList_CreateNode(data);
	}
}

void LinkedList_Delete(LinkedList* list, void* data)
{
	LinkedListNode * current = list->head;
	LinkedListNode * previous = current;
	while(current != 0)
	{
		if(current->data == data)
		{
			previous->next = current->next;
			if(current == list->head)
			{
				list->head = current->next;
			}
			Memory_Free(current);
			return;
		}
		previous = current;
		current = current->next;
	}
}

void LinkedList_Reverse(LinkedList* list)
{
	LinkedListNode* reversed = 0;
	LinkedListNode* current = list->head;
	LinkedListNode* temp = 0;
	while(current != 0)
	{
		temp = current;
		current = current->next;
		temp->next = reversed;
		reversed = temp;
	}
	list->head = reversed;
}

void* LinkedList_At(LinkedList* list, unsigned int index)
{
	LinkedListNode* current = list->head;
    unsigned int i;
	for (i=0; i<index; i++)
    {
        current = current->next;
        if (!current)
        {
            return 0;
        }
    }
    return current->data;
}


LinkedListNode* LinkedList_CreateNode(void* data)
{
	LinkedListNode* newNode = Memory_Allocate(sizeof(LinkedListNode));
	if (!newNode)
	{
		return 0;
	}
	newNode->data = data;
	newNode->next = 0;
	return newNode;
}


unsigned int LinkedList_Size(LinkedList* list)
{
    unsigned int size = 0;
    LinkedListNode* current = list->head;
    while (current)
    {
        size++;
        current = current->next;
    }
    return size;
}
