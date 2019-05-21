#pragma once

#include <stdbool.h>
#include <stdint.h>

struct LinkedListNode
{
  void* Data;
  struct LinkedListNode* Next;
};

typedef struct LinkedListNode LinkedListNode;

struct LinkedList
{
  LinkedListNode* Head;
};

typedef struct LinkedList LinkedList;

void LinkedList_Constructor(LinkedList* self);
void LinkedList_Destructor(LinkedList* self);
void LinkedList_PushBack(LinkedList* self, void* data);
void LinkedList_Delete(LinkedList* self, void* data);
void LinkedList_Display(LinkedList* self);
void LinkedList_Reverse(LinkedList* self);
LinkedListNode* LinkedList_CreateNode(LinkedList* self, void* data);
void LinkedList_FreeAllData(LinkedList* self);

uint32_t LinkedList_Size(LinkedList* self);
bool LinkedList_IsEmpty(LinkedList* self);
void* LinkedList_At(LinkedList* self, uint32_t index);
void* LinkedList_PopFront(LinkedList* self);
