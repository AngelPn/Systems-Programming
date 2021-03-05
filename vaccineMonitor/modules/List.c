#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../include/List.h"

// List is pointer to this struct
struct list
{
	ListNode dummy;
	ListNode last;
	int length;
	DestroyFunc destroy_item;
};

struct list_node
{
	void *item;
	ListNode next;
};

List list_create(DestroyFunc destroy_item)
{
	List list = malloc(sizeof(struct list));
	list->length = 0;
	list->destroy_item = destroy_item;
	list->dummy = malloc(sizeof(struct list_node));
	list->dummy->next = NULL;
	list->last = list->dummy;
	return list;
}

int list_length(List list)
{
	return list->length;
}

ListNode list_first(List list)
{
	return list->dummy->next;
}

ListNode list_last(List list)
{
	if (list->last == list->dummy)
		return NULL; // empty list
	else
		return list->last;
}

void *list_insert_next(List list, ListNode node, void *item)
{
	if (node == NULL)
		node = list->dummy;

	ListNode new_node = malloc(sizeof(struct list_node));
	new_node->item = item;
	new_node->next = node->next;
	node->next = new_node;

	list->length++;
	if (list->last == node)
		list->last = new_node;

	return new_node;
}

// int compare_integers(int k, int previous, int next){
// 	if (k > previous && k < next) return 0;
// 	else return -1;
// }

// int compare_strings(char *k, char *previous, char *next){
// 	if (strcmp(k, previous) > 0 && strcmp(k, next) < 0) return 0;
// 	else return -1;
// }

// void list_insert_ordered(List list, void *item, int order_type, GetKey key){
// 	if (order_type = Integer){
// 		int k = key(item);
// 		if (k > key(list_node_item(list, list_last(list))))
// 			list_insert_next(list, list_last(list), item);
// 		else{
// 			for (ListNode node = list->dummy->next; node != NULL; node = node->next){
// 				if (compare_integers(k, key(node->item), key(list_next(list, node))) == 0){
// 					list_insert_next(list, node, item);
// 					break;
// 				}
// 			}
// 		}
// 	}
// 	else{
// 		char *k = key(item);
// 		if (strcmp(k, key(list_node_item(list, list_last(list)))) > 0)
// 			list_insert_next(list, list_last(list), item);
// 		else{
// 			for (ListNode node = list->dummy->next; node != NULL; node = node->next){
// 				if (compare_strings(k, key(node->item), key(list_next(list, node))) == 0){
// 					list_insert_next(list, node, item);
// 					break;
// 				}
// 			}
// 		}
// 	}
// }

void list_remove_next(List list, ListNode node)
{
	if (node == NULL)
		node = list->dummy;

	ListNode removed = node->next;
	assert(removed != NULL); // LCOV_EXCL_LINE

	if (list->destroy_item != NULL)
		list->destroy_item(removed->item);

	node->next = removed->next;
	free(removed);

	list->length--;
	if (list->last == removed)
		list->last = node;
}

void list_remove(List list, ListNode node)
{
	ListNode prev_node = list->dummy;
	for (ListNode temp = list_first(list); temp != NULL; temp = list_next(list, temp))
	{
		if (temp == node)
		{
			list_remove_next(list, prev_node);
			return;
		}
		prev_node = temp;
	}
}

ListNode list_find_node(List list, void *item, CompareFunc compare)
{
	for (ListNode node = list->dummy->next; node != NULL; node = node->next)
		if (compare(item, node->item) == 0)
			return node; // found
	return NULL;		 //not found
}

void *list_find(List list, void *item, CompareFunc compare)
{
	ListNode node = list_find_node(list, item, compare);
	return node == NULL ? NULL : node->item;
}

void *list_find_order(List list, ListNode node, void *key, CompareFunc compare, bool *found)
{
	ListNode begin_node;
	if (node == NULL)
		begin_node = list->dummy->next;
	else
		begin_node = node;

	for (ListNode node = begin_node; node != NULL; node = node->next){

		int compared = compare(key, node->item);
		if (compared == 0){
			*found = true;
			return node;
		}
		else if (compared < 0)
			return NULL;
		else{
			ListNode next_node = node->next;
			if (next_node != NULL){
				if (compare(key, node->item) > 0 && compare(key, next_node->item) < 0)
					return node;
				else
					continue;
			}
			else{
				if (compare(key, node->item) > 0)
					return node;
				else
					return NULL;
			}
		}
	}
	return NULL;
}

DestroyFunc list_set_destroy_item(List list, DestroyFunc destroy_item)
{
	DestroyFunc old = list->destroy_item;
	list->destroy_item = destroy_item;
	return old;
}

void list_destroy(List list)
{
	ListNode node = list->dummy;
	while (node != NULL)
	{
		ListNode next = node->next;

		if (node != list->dummy && list->destroy_item != NULL)
			list->destroy_item(node->item);

		free(node);
		node = next;
	}
	free(list);
}

ListNode list_next(List list, ListNode node)
{
	assert(node != NULL);
	return node->next;
}

void *list_node_item(List list, ListNode node)
{
	assert(node != NULL);
	return node->item;
}

void list_append(List list, List to_append)
{
	if (list_length(to_append))
	{
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
void list_print(List list, PrintItem print)
{
	for (ListNode node = list->dummy->next; node != NULL; node = node->next)
		print(node->item);
}