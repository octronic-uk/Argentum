#pragma once

struct MemoryDriver;

struct LinkedListNode
{
  void* Data;
  struct LinkedListNode* Next;
};

struct LinkedList
{
  struct MemoryDriver* Memory;
  struct LinkedListNode* Head;
};

void LinkedList_Constructor(struct LinkedList* self, struct MemoryDriver* memory);
void LinkedList_Destructor(struct LinkedList* self);
void LinkedList_PushBack(struct LinkedList* self, void* data);
void LinkedList_Delete(struct LinkedList* self, void* data);
void LinkedList_Display(struct LinkedList* self);
void LinkedList_Reverse(struct LinkedList* self);
struct LinkedListNode* LinkedList_CreateNode(struct LinkedList* self, void* data);

unsigned int LinkedList_Size(struct LinkedList* self);
void* LinkedList_At(struct LinkedList* self, unsigned int index);
