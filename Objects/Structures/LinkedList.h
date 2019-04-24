#pragma once

#include <stdbool.h>
#include <stdint.h>

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
void LinkedList_FreeAllData(struct LinkedList* self);

uint32_t LinkedList_Size(struct LinkedList* self);
bool LinkedList_IsEmpty(struct LinkedList* self);
void* LinkedList_At(struct LinkedList* self, uint32_t index);
