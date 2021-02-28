#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "common_types.h"
#include "List.h"

//"struct skiplist" and "struct skiplist_node" are incomplete structs
typedef struct skiplist *SkipList;
typedef struct skiplist_node *SLNode;

SkipList SLCreate_with_maxlevel(int max_level, float p, DestroyFunc destroy_item);
SkipList SLCreate(float p, DestroyFunc destroy_item);
void *SLSearch(SkipList sl, void *key, CompareFunc compare);
void SLInsert(SkipList sl, void *item, GetKey key, CompareFunc compare);
// void SLRemove(SkipList *psl, KeyType key);
void SLPrint(SkipList sl, PrintItem print);
void SLDestroy(SkipList sl);

#endif
