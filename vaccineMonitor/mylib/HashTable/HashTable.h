#ifndef HT_H
#define HT_H

#include "List.h"

/* struct hashtable is incomplete type */
typedef struct hashtable *HashTable;

HashTable HTCreate(int keytype, DestroyFunc destroy_item);
int HTSize(HashTable ht);
void *HTSearch(HashTable ht, void *key, CompareFunc compare);
void HTInsert(HashTable *pht, void *item, GetKey key);
void HTVisit(HashTable ht, VisitFunc visit, int key);
void HTPrint(HashTable ht, PrintItem print);
void HTDestroy(HashTable ht);

#endif