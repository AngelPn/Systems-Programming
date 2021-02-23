#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../include/list.h"

// list implementation

LLNode NewNode(LLNode next) { // auxiliary function to create a new node with a given next
	LLNode node = (LLNode)malloc(sizeof(*node));
	node->next = next;
	return node;
}

LList *LLCreate(int item_size) {
	LList *plist = (LList *)malloc(sizeof(LList));
	plist->dummy = NewNode(NULL);
	plist->item_size = item_size;
	
	return plist;
}

void LLDestroy(LList list) {
	LLNode node = list.dummy;
	while(node != NULL) {
		LLNode next = node->next;		// get this before free()!
		free(node->item);				// free the item, not just the node!
		free(node);
		node = next;
	}
}

LLNode LLNext(LList list, LLNode node) {
	return node == NULL ? list.dummy->next : node->next;
}

void *LLGetItem(LList list, LLNode node) {
	assert(node != NULL);
	return node->item;
}

LLNode LLInsertAfter(LList list, LLNode node, void *item) {
	if(node == NULL)		// if node is null then
		node = list.dummy;	// insert after the dummy! (

	node->next = NewNode(node->next);
	node->next->item = malloc(list.item_size);
	memcpy(node->next->item, item, list.item_size);
	return node->next;
}

void LLRemove(LList list, LLNode node) {
	LLNode prev = list.dummy;
	while(prev->next != NULL){
		if(prev->next == node){
			prev->next = node->next;
            free(node->item); // free the item, not just the node!
			free(node);
			break;
		}
		prev = prev->next;
	}
}

LLNode LLFind(LList list, void *item, LLCompareFunc compare) {
	// iterate over list
	for(LLNode node = list.dummy->next; node != NULL; node = node->next)
		if(compare(item, node->item) == 0)
			return node;		// found it

	return NULL;	// it's not here
}

void LLVisit(LList list, LLVisitFunc visit) {
	// iterate over list
	for(LLNode node = list.dummy->next; node != NULL; node = node->next)
		visit(list, node);
}