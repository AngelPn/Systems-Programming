/*File: HashTable.h*/
#pragma once

#include "ADTList.h"

typedef char *KeyType;

typedef struct htitem{
    int dummy;
} HTItem;

typedef struct entry{
	char *Key;
	HTItem Item;
} Entry;

typedef struct{
	int size; /*size of the array*/
	int n; /*n is the number of entries*/
	List *HashTable; /*A Hash Table of pointers to untyped linked list*/
} HTHash;

typedef void (*HTvisit)(HTHash *hash, KeyType key, HTItem item);

HTHash *HTCreate();
int HTSize(HTHash *hash);
int HTGet(HTHash *hash, KeyType key, HTItem *pitem);
void HTInsert(HTHash **phash, KeyType key, HTItem item);
void HTRemove(HTHash *phash, KeyType key);
void HTVisit(HTHash *hash, HTvisit Function);
void HTDestroy(HTHash *phash);

int compare_keys(void *a, void *b);