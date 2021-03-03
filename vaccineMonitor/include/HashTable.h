#ifndef HT_H
#define HT_H

#include "common_types.h"
#include "List.h"

typedef struct hashtable *HashTable;

HashTable HTCreate(int keytype, DestroyFunc destroy_item);
int HTSize(HashTable ht);
void *HTSearch(HashTable ht, void *key, CompareFunc compare);
void HTInsert(HashTable *pht, void *item, GetKey key);
// void HTRemove(HashTable *pht, KeyType key);
void HTPrint(HashTable ht, PrintItem print);
void HTDestroy(HashTable ht);

#endif