#ifndef HT_H
#define HT_H

#include "List.h"
#include "common_types.h"

enum key_type {String, Integer} KeyType;

typedef struct hashtable *HashTable;

HashTable HTCreate(int keytype, DestroyFunc destroy_item);
int HTSize(HashTable hash);
void *HTSearch(HashTable hash, void *key, CompareFunc compare);
void HTInsert(HashTable hash, void *item, GetKey key);
// void HTRemove(HashTable *phash, KeyType key);
void HTPrint(HashTable hash, PrintItem print);
void HTDestroy(HashTable hash);

int compare_keys(void *a, void *b);



#endif