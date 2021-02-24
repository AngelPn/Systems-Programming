#ifndef LIST_H
#define LIST_H

#include "common_types.h"

//"struct list" and "struct list_node" are incomplete structs
typedef struct list *List;
typedef struct list_node *ListNode;

//Creates and returns a new list. If destroy_item!=NULL, the destroy_item(item) is called every time an item is removed
List list_create(DestroyFunc destroy_item);

int list_length(List list);

//Returns the first and the last node of list or LIST_BOF / LIST_EOF if list is empty
ListNode list_first(List list);
ListNode list_last(List list);

//Insert new node after ListNode node or in the beginning if node == LIST_BOF
void list_insert_next(List list, ListNode node, void * item);

//Removes the next node from ListNode node
void list_remove_next(List list, ListNode node);

void list_remove(List list, ListNode node);

ListNode list_find_node(List list, void * item, CompareFunc compare);

//Returns the first item found equal with item
void * list_find(List list, void * item, CompareFunc compare);

DestroyFunc list_set_destroy_item(List list, DestroyFunc destroy_item);

//Deallocates list
void list_destroy(List list);

//Returns the node after the ListNode node or LIST_EOF if the node was the last
ListNode list_next(List list, ListNode node);

//Returns content of node
void *list_node_item(List list, ListNode node);

void list_append(List list, List to_append);

void list_print(List list, PrintItem print);

#endif
