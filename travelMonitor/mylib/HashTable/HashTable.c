#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "HashTable.h"

struct hashtable
{
	int size; 		/*size of the array*/
	int n; 			/*n is the number of entries*/
	List *chains; 	/*Hash Table is array of pointers to untyped linked list*/
	int keytype; 	/*determines type of key: String or Integer*/
	DestroyFunc destroy_item;
};

HashTable HTCreateHash(int size, int keytype, DestroyFunc destroy_item){

	HashTable ht = (HashTable)malloc(sizeof(struct hashtable));

	ht->size = size;
	ht->n = 0;
	ht->chains = (List *)malloc(sizeof(List)*size);
	ht->destroy_item = destroy_item;
	ht->keytype = keytype;

	if(ht->chains == NULL){
		printf("Sorry! System storage is exchausted\n");
		exit(EXIT_FAILURE);
	}

    /*Create an array of lists of size 'size': Initialize the Hash Table with NULL*/
	for(int i = 0; i < size; i++)
		ht->chains[i] = NULL;
	return ht;
}

HashTable HTCreate(int keytype, DestroyFunc destroy_item){
	return HTCreateHash(7, keytype, destroy_item);
}

int HTSize(HashTable ht){
	return ht->n;
}

List get_HTchain(HashTable ht, int i){
	return ht->chains[i];
}

/* Hash function: takes into account the type of key: String or Integer */
int Hash(HashTable ht, int keytype, void *key){
	if (keytype == String){
		char *k = key;
		int h=0, a=33;
		for(; *k!='\0'; k++)
			h=(a*h + *k)%(ht->size);
		return h;		
	}
	else{
		int k = *(int *)key;
		return k%(ht->size);
	}
}

void *HTSearch(HashTable ht, void *key, CompareFunc compare){

	int h = Hash(ht, ht->keytype, key); /*Find the index of key*/
	List head = ht->chains[h]; /*head is the linked list of the key we are searching for*/

	if (head != NULL)
		return list_find(head, key, compare);		
	else
		return NULL;
}

/* Destroys old Hash Table in case of rehashing */
/* Does not actually destroys the item because new Hash Table points to it */
void HTDestroyOldHT(HashTable ht){
	List head = NULL;

	for (int i = 0; i < ht->size; i++){
		head = ht->chains[i];

		if( head != NULL){
			list_set_destroy_item(head, NULL);
			list_destroy(head);
		}
	}
	free(ht->chains);
	free(ht);
}

/* Returns new Hash Table with doubled size of array and destroys the old */
HashTable Rehashing(HashTable ht, GetKey key){

	/*Create a hash with the new size*/
	HashTable NewHash = HTCreateHash((ht->size)*2, ht->keytype, ht->destroy_item);

	List head = NULL;

	for (int i = 0; i < ht->size; i++){
		head = ht->chains[i];

		if(head != NULL){
			ListNode curr_node = list_first(head);
			while( curr_node != NULL){
				void *node_item = list_node_item(head, curr_node);

				HTInsert(&NewHash, node_item, key);
				curr_node = list_next(head, curr_node);
			}
		}
	}
	HTDestroyOldHT(ht);
	return NewHash;
}

void HTInsert(HashTable *pht, void *item, GetKey key){
	HashTable ht = *pht;
	
	(ht->n)++;/*Increase the counter of entries*/
	double LF= ((double) ht->n) / ((double) ht->size); /*Calculate the load factor*/

	int h = Hash(ht, ht->keytype, key(item)); /*Find the index of key*/

	List head = NULL; 
	/*head points at the linked list we are going to insert the item*/
	if ( (head = ht->chains[h]) == NULL){
		ht->chains[h] = list_create(ht->destroy_item);
		head = ht->chains[h];
	}
	
	list_insert_next(head, list_last(head), item);

	if(LF >= 0.9){ /*Double the size of the array*/
		*pht = Rehashing(ht, key);
	}
}

void HTVisit(HashTable ht, VisitFunc visit, int key){

	List head = NULL;

	for (int i = 0; i < ht->size; i++){
		head = ht->chains[i];
		if(head != NULL){
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
				visit(list_node_item(head, node), key);
			}
		}		
	}
}

void HTPrint(HashTable ht, PrintItem print){

	List head = NULL;

	for (int i = 0; i < ht->size; i++){
		head = ht->chains[i];
		if(head != NULL){
			list_print(head, print);
		}		
	}
}

void HTDestroy(HashTable ht){
	List head = NULL;

	for (int i = 0; i < ht->size; i++){
		head = ht->chains[i];
			
		if(head != NULL){
			list_destroy(head);
		}
	}
	free(ht->chains);
	free(ht);
}