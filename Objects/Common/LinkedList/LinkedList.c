#include "LinkedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;

void LinkedList_Constructor(LinkedList* self)
{
	self->Head = 0;
}

void LinkedList_Destructor(LinkedList* self)
{
	LinkedListNode* current = self->Head;
	LinkedListNode* next = current;
	while(current != 0)
	{
		next = current->Next;
		free(current);
		current = next;
	}
}

void LinkedList_Display(LinkedList* self)
{
	LinkedListNode* current = self->Head;
	if(self->Head == 0)
	{
		return;
	}

	for(; current != 0; current = current->Next)
	{
		printf("%d\n", (uint32_t)current->Data);
	}
}

void LinkedList_PushBack(LinkedList* self, void* data)
{
	LinkedListNode* current = 0;
	if(self->Head == 0)
	{
		self->Head = LinkedList_CreateNode(self,data);
	}
	else
	{
		current = self->Head;
		while (current->Next != 0)
		{
			current = current->Next;
		}
		current->Next = LinkedList_CreateNode(self, data);
	}
}

void LinkedList_Delete(LinkedList* self, void* data)
{
	LinkedListNode* current = self->Head;
	LinkedListNode* previous = current;
	while(current != 0)
	{
		if(current->Data == data)
		{
			previous->Next = current->Next;
			if(current == self->Head)
			{
				self->Head = current->Next;
			}
			free(current);
			return;
		}
		previous = current;
		current = current->Next;
	}
}

void LinkedList_Reverse(LinkedList* self)
{
	LinkedListNode* reversed = 0;
	LinkedListNode* current = self->Head;
	LinkedListNode* temp = 0;
	while(current != 0)
	{
		temp = current;
		current = current->Next;
		temp->Next = reversed;
		reversed = temp;
	}
	self->Head = reversed;
}

void* LinkedList_At(LinkedList* self, uint32_t index)
{
	LinkedListNode* current = self->Head;

	if (!current)
	{
		return 0;
	}

    uint32_t i;
	for (i=0; i<index; i++)
    {
        current = current->Next;
        if (!current)
        {
            return 0;
        }
    }
    return current->Data;
}


LinkedListNode* LinkedList_CreateNode(LinkedList* self, void* data)
{
	LinkedListNode* newNode = malloc(sizeof(LinkedListNode));
	if (!newNode)
	{
		return 0;
	}
	newNode->Data = data;
	newNode->Next = 0;
	return newNode;
}

uint32_t LinkedList_Size(LinkedList* self)
{
    uint32_t size = 0;
    LinkedListNode* current = self->Head;
    while (current)
    {
        size++;
        current = current->Next;
    }
    return size;
}

bool LinkedList_IsEmpty(LinkedList* self)
{
	return LinkedList_Size(self) == 0;
}

void LinkedList_FreeAllData(LinkedList* self)
{
	LinkedListNode* current = self->Head;
	while(current)
	{
		free(current->Data);
		current = current->Next;
	}
}

void* LinkedList_PopFront(LinkedList* self)
{
	LinkedListNode* current = self->Head;
	void* data = current->Data;

	if (current->Next)
	{
		self->Head = current->Next;
	}
	else
	{
		self->Head = 0;
	}
	
	free(current);

	return data;
}