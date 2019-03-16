#pragma once


struct LinkedListNode_t
{
  void* data;
  struct LinkedListNode_t* next;
};


struct LinkedList_t
{
  struct LinkedListNode_t* head;
};

typedef struct LinkedListNode_t LinkedListNode;
typedef struct LinkedList_t LinkedList;

LinkedList* LinkedList_Constructor();
void LinkedList_Destructor(LinkedList* list);
void LinkedList_PushBack(LinkedList* list, void* data);
void LinkedList_Delete(LinkedList* list, void* data);
void LinkedList_Display(LinkedList* list);
void LinkedList_Reverse(LinkedList* list);
LinkedListNode* LinkedList_CreateNode(void* data);

unsigned int LinkedList_Size(LinkedList* list);
void* LinkedList_At(LinkedList* list, unsigned int index);
