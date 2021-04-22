#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "List.h"

/* "struct skiplist" and "struct skiplist_node" are incomplete structs */
typedef struct skiplist *SkipList;
typedef struct skiplist_node *SLNode;

/* Creates and returns a new Skip List. 
    If destroy_item!=NULL, the destroy_item(item) is called every time an item is removed */
SkipList SLCreate_with_maxlevel(int max_level, float p, DestroyFunc destroy_item);
SkipList SLCreate(float p, DestroyFunc destroy_item);

/* Returns pointer to list of Layer 0 */
List get_bottom_level(SkipList sl);

/* Searches key in Skip List and returns item, if found. If not, returns NULL
    Note that different key types need different specifier to compare keys */
void *SLSearch(SkipList sl, void *key, CompareFunc compare);

/* Inserts item in Skip List with key specified by GetKey */
/* Compares item's key with compare func */
/* If item is already in skip list, then dismiss insertion */
void SLInsert(SkipList sl, void *item, GetKey key, CompareFunc compare);

/* Removes item specified by key */
void SLRemove(SkipList sl, void *key, CompareFunc compare);

/* Prints Skip List */
void SLPrint_BottomLevel(SkipList sl, PrintItem print);
void SLPrint(SkipList sl, PrintItem print);

/* Destroys Skip List */
void SLDestroy(SkipList sl);

#endif
