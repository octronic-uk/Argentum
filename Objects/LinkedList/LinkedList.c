#include "LinkedList.h"

#include <stdio.h>
#include <string.h>


#include <Objects/Kernel/Kernel.h>
#include <Drivers/Memory/MemoryDriver.h>

extern struct Kernel _Kernel;

void LinkedList_Constructor(struct LinkedList* self)
{
	self->Head = 0;
}

void LinkedList_Destructor(struct LinkedList* self)
{
	struct LinkedListNode* current = self->Head;
	struct LinkedListNode* next = current;
	while(current != 0)
	{
		next = current->Next;
		MemoryDriver_Free(&_Kernel.Memory, current);
		current = next;
	}
}

void LinkedList_Display(struct LinkedList* self)
{
	struct LinkedListNode* current = self->Head;
	if(self->Head == 0)
	{
		return;
	}

	for(; current != 0; current = current->Next)
	{
		printf("%d\n", (uint32_t)current->Data);
	}
}

void LinkedList_PushBack(struct LinkedList* self, void* data)
{
	struct LinkedListNode* current = 0;
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

void LinkedList_Delete(struct LinkedList* self, void* data)
{
	struct LinkedListNode* current = self->Head;
	struct LinkedListNode* previous = current;
	while(current != 0)
	{
		if(current->Data == data)
		{
			previous->Next = current->Next;
			if(current == self->Head)
			{
				self->Head = current->Next;
			}
			MemoryDriver_Free(&_Kernel.Memory, current);
			return;
		}
		previous = current;
		current = current->Next;
	}
}

void LinkedList_Reverse(struct LinkedList* self)
{
	struct LinkedListNode* reversed = 0;
	struct LinkedListNode* current = self->Head;
	struct LinkedListNode* temp = 0;
	while(current != 0)
	{
		temp = current;
		current = current->Next;
		temp->Next = reversed;
		reversed = temp;
	}
	self->Head = reversed;
}

void* LinkedList_At(struct LinkedList* self, uint32_t index)
{
	struct LinkedListNode* current = self->Head;

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


struct LinkedListNode* LinkedList_CreateNode(struct LinkedList* self, void* data)
{
	struct LinkedListNode* newNode = MemoryDriver_Allocate(&_Kernel.Memory, sizeof(struct LinkedListNode));
	if (!newNode)
	{
		return 0;
	}
	newNode->Data = data;
	newNode->Next = 0;
	return newNode;
}

uint32_t LinkedList_Size(struct LinkedList* self)
{
    uint32_t size = 0;
    struct LinkedListNode* current = self->Head;
    while (current)
    {
        size++;
        current = current->Next;
    }
    return size;
}

bool LinkedList_IsEmpty(struct LinkedList* self)
{
	return LinkedList_Size(self) == 0;
}

void LinkedList_FreeAllData(struct LinkedList* self)
{
	struct LinkedListNode* current = self->Head;
	while(current)
	{
		MemoryDriver_Free(&_Kernel.Memory, current->Data);
		current = current->Next;
	}
}