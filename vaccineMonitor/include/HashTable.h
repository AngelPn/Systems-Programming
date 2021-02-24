#ifndef HT_H
#define HT_H

#include "List.h"
#include "common_types.h"

typedef char *KeyType;

typedef struct entry{
	KeyType key;
	void *item;
} bucket;

typedef struct{
	int size; /*size of the array*/
	int n; /*n is the number of entries*/
	List *HashTable; /*A Hash Table of pointers to untyped linked list*/
} HTHash;

typedef void (*HTvisit)(HTHash *hash, KeyType key, void item);

HTHash *HTCreate();
int HTSize(HTHash *hash);
void *HTSearch(HTHash *hash, KeyType key);
void HTInsert(HTHash **phash, KeyType key, void item);
void HTRemove(HTHash *phash, KeyType key);
// void HTVisit(HTHash *hash, HTvisit Function);
void HTDestroy(HTHash *phash);

int compare_keys(void *a, void *b);

void destroy_bucket(void *b);

#endif