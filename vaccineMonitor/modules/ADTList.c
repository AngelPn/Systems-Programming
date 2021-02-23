#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../include/ADTList.h"

// List is pointer to this struct
struct list{
	ListNode dummy;
	ListNode last;
	int length;	
	DestroyFunc destroy_item;
};

struct list_node{
	void *item;
	ListNode next;
};


List list_create(DestroyFunc destroy_item){
	List list = malloc(sizeof(struct list));
	list->length = 0;
	list->destroy_item = destroy_item;
	list->dummy = malloc(sizeof(struct list_node));
	list->dummy->next = NULL;
	list->last = list->dummy;
	return list;
}

int list_length(List list){
	return list->length;
}

ListNode list_first(List list){
	return list->dummy->next;
}

ListNode list_last(List list){
	if (list->last == list->dummy)
		return NULL;		// empty list
	else
		return list->last;
}

void list_insert_next(List list, ListNode node, void * item, size_t item_sz){
	if (node == NULL)
		node = list->dummy;

	ListNode new_node = malloc(sizeof(struct list_node));
	new_node->item = malloc(sizeof(item_sz));
	memcpy(new_node->item, item, item_sz);

	new_node->next = node->next;
	node->next = new_node;

	list->length++;
	if (list->last == node)
		list->last = new_node;
}

void list_remove_next(List list, ListNode node){
	if (node == NULL)
		node = list->dummy;

	ListNode removed = node->next;
	assert(removed != NULL);		// LCOV_EXCL_LINE

	if (list->destroy_item != NULL)
		list->destroy_item(removed->item);

	node->next = removed->next;
	free(removed);

	list->length--;
	if (list->last == removed)
		list->last = node;
}

void list_remove(List list, ListNode node){
	ListNode prev_node = list->dummy;
	for(ListNode temp = list_first(list); temp != LIST_EOF; temp = list_next(list, temp)) {      
		if(temp == node){
			list_remove_next(list, prev_node);
			return;
		}
		prev_node = temp;
	}
}

ListNode list_find_node(List list, void *item, CompareFunc compare){
	for (ListNode node = list->dummy->next; node != NULL; node = node->next)
		if (compare(item, node->item) == 0)
			return node;// found
	return NULL;	//not found
}

void *list_find(List list, void *item, CompareFunc compare){
	ListNode node = list_find_node(list, item, compare);
	return node == NULL ? NULL : node->item;
}

DestroyFunc list_set_destroy_item(List list, DestroyFunc destroy_item){
	DestroyFunc old = list->destroy_item;
	list->destroy_item = destroy_item;
	return old;
}

void list_destroy(List list){
	ListNode node = list->dummy;
	while (node != NULL) {
		ListNode next = node->next;

		if (node != list->dummy && list->destroy_item != NULL)
			list->destroy_item(node->item);

		free(node);
		node = next;
	}
	free(list);
}

ListNode list_next(List list, ListNode node){
	assert(node != NULL);	// LCOV_EXCL_LINE
	return node->next;
}

void * list_node_item(List list, ListNode node){
	assert(node != NULL);	// LCOV_EXCL_LINE
	return node->item;
}

void list_append(List list, List to_append){
	if(list_length(to_append)){
		list->last->next = list_first(to_append);
		list->length += list_length(to_append);
		list->last = list_last(to_append);
	}
	to_append->dummy->next = NULL;
	to_append->last = to_append->dummy;
	list_destroy(to_append);
}

/* Function to print nodes in a given linked list.
   Note that different data types need different specifier in printf() */
void list_print(List list, PrintItem print){
	for (ListNode node = list->dummy->next; node != NULL; node = node->next)
		print(node->item); 
} 