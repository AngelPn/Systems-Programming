#ifndef LIST_H
#define LIST_H
// A simple untyped linked list

#define LL_NIL NULL					// A "Nil" node is just NULL

// Types

struct ll_node{
	void *item;							// pointer to our item (whose type is unknown)
	struct ll_node *next;
};

typedef struct ll_node *LLNode;			// LLNode is a pointer to "struct ll_node"

typedef struct {						// LList is a struct
	LLNode dummy;						// we use a dummy node, store it here
	int item_size;						// the size of our items
} LList;

typedef int (*LLCompareFunc )(void*, void*);	// LLCompareFunc: type of functions that compare two items
typedef void (*LLVisitFunc )(LList, LLNode);	// LLVisitFunc: type of functions that visit a node


// Function declarations

// creates a new list. The item is untyped so we need to know its size!

LList *LLCreate(int item_size);

// destroys a list

void LLDestroy(LList list);


// returns the node after 'node'. Use node = LL_NIL to get the first.

LLNode LLNext(LList list, LLNode node);

// Returns a pointer to the data

void *LLGetItem(LList list, LLNode node);

// Inserts node after given one and returns it. Use node = LL_NIL to insert as first.

LLNode LLInsertAfter(LList list, LLNode node, void *item);

// Removes a node

void LLRemove(LList list, LLNode node);

// Returns a node that matches the given item (or Nil if not found)

LLNode LLFind(LList list, void *item, LLCompareFunc compare);

// Calls visit(list, node) on every node

void LLVisit(LList list, LLVisitFunc visit);


#endif