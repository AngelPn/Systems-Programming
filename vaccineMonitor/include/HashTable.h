#ifndef HT_H
#define HT_H

#include "List.h"
#include "common_types.h"

typedef char *KeyType;
typedef struct hashtable *HashTable;
typedef struct entry *bucket;

struct entry{
	KeyType key;
	void *item;
};

struct hashtable{
	int size; /*size of the array*/
	int n; /*n is the number of entries*/
	List *chains; /*A Hash Table of pointers to untyped linked list*/
	DestroyFunc destroy_item;
};

bucket create_bucket(KeyType key, void *item);
void destroy_bucket(void *b);

HashTable HTCreate(DestroyFunc destroy_item);
int HTSize(HashTable hash);
void *HTSearch(HashTable hash, KeyType key);
void HTInsert(HashTable hash, KeyType key, void *item);
// void HTRemove(HashTable *phash, KeyType key);
void HTPrint(HashTable hash, PrintItem print);
void HTDestroy(HashTable hash);

int compare_keys(void *a, void *b);



#endif