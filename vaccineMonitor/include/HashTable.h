#ifndef HT_H
#define HT_H

#include "common_types.h"
#include "List.h"

typedef struct hashtable *HashTable;

HashTable HTCreate(int keytype, DestroyFunc destroy_item);
int HTSize(HashTable hash);
void *HTSearch(HashTable hash, void *key, CompareFunc compare);
void HTInsert(HashTable *phash, void *item, GetKey key);
// void HTRemove(HashTable *phash, KeyType key);
void HTPrint(HashTable hash, PrintItem print);
void HTDestroy(HashTable hash);

#endif