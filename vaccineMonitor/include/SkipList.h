#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "common_types.h"
#include "List.h"

//"struct list" and "struct list_node" are incomplete structs
typedef struct skiplist *SkipList;

SkipList SLCreate(float p, DestroyFunc destroy_item);
void *SLSearch(SkipList sl, void *key, CompareFunc compare);
void SLInsert(SkipList sl, void *item, GetKey key);
// void SLRemove(SkipList *psl, KeyType key);
void SLPrint(SkipList sl, PrintItem print);
void SLDestroy(SkipList sl);

#endif
