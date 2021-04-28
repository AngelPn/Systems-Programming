#ifndef HT_H
#define HT_H

#include "List.h"

/* enumeration to determine the key type of hash table */
enum key_type {String, Integer} KeyType;

/* struct hashtable is incomplete type */
typedef struct hashtable *HashTable;

/* Creates and returns a new Hash Table with type of key given in args. 
    If destroy_item!=NULL, the destroy_item(item) is called every time an item is removed */
HashTable HTCreate(int keytype, DestroyFunc destroy_item);

/* Returns the size of Hash Table */
int HTSize(HashTable ht);

/* Returns the number of elements in the Hash Table */
int HTEntries(HashTable ht);

/* Returns the i-th chain of Hash Table */
List get_HTchain(HashTable ht, int i);

/* Searches key in Hash Table and returns item, if found. If not, returns NULL
    Note that different key types need different specifier to compare keys */
void *HTSearch(HashTable ht, void *key, CompareFunc compare);

/* Inserts item in Hash Table with key specified by GetKey */
void HTInsert(HashTable *pht, void *item, GetKey key);

/* Visits every node of Hash Table and calls visit function passed with arg key */
void HTVisit(HashTable ht, VisitFunc visit, int key);

/* Prints Hash Table */
void HTPrint(HashTable ht, PrintItem print);

/* Destroys Hash Table */
void HTDestroy(HashTable ht);

#endif