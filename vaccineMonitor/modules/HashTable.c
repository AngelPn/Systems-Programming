/*File: HashTable.c*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/HashTable.h"

struct hashtable{
	int size; /*size of the array*/
	int n; /*n is the number of entries*/
	List *chains; /*A Hash Table of pointers to untyped linked list*/
	int keytype;
	DestroyFunc destroy_item;
};

HashTable HTCreateHash(int size, int keytype, DestroyFunc destroy_item){
	HashTable hash = (HashTable)malloc(sizeof(struct hashtable));

	hash->size = size;
	hash->n = 0;
	hash->chains = (List *)malloc(sizeof(List)*size);
	hash->destroy_item = destroy_item;
	hash->keytype = keytype;

	if(hash->chains == NULL){
		printf("Sorry! System storage is exchausted\n");
		exit(-1);
	}

    /*Create an array of lists of size 'size': Initialize the Hash Table with NULL*/
	for(int i = 0; i < size; i++)
		hash->chains[i] = NULL;
	return hash;
}

HashTable HTCreate(int keytype, DestroyFunc destroy_item){
	return HTCreateHash(7, keytype, destroy_item);
}

int HTSize(HashTable hash){ /*Function that returns the number of elements in the hash*/
	return hash->n; /*n is the number of entries! */
}

int Hash(HashTable hash, int keytype, void *key){/*Function that counts the index*/
	if (keytype == String){
		char *k = key;
		int h=0, a=33;
		for(; *k!='\0'; k++)
			h=(a*h + *k)%(hash->size);
		return h;		
	}
	else{
		int k = *(int *)key;
		return k%(hash->size);
	}

}


/*Function that searches the key in the hash and returns the pointer to this bucket*/
void *HTSearch(HashTable hash, void *key, CompareFunc compare){

	int h = Hash(hash, hash->keytype, key); /*Find the index of key*/
	List head = hash->chains[h]; /*head is the linked list of the key we are searching for*/
    //List head = *(hash->chains[h]);

	if (head != NULL){
		void *pitem = list_find(head, key, compare);
		if (pitem == NULL) return NULL;
		else return pitem;		
	}
	else return NULL;

}

void HTDestroyOldHT(HashTable hash);

HashTable Reharshing(HashTable hash, GetKey key){

	HashTable NewHash= HTCreateHash((hash->size)*2, hash->keytype, hash->destroy_item); /*Create a hash with the new size*/

	List head = NULL;

	for (int i = 0; i < hash->size; i++){
		head = hash->chains[i];
		if(head != NULL){

			ListNode curr_node = list_first(head);
			while( curr_node != NULL){
				void *node_item = list_node_item(head, curr_node);

				HTInsert(NewHash, node_item, key(node_item));
				curr_node = list_next(head, curr_node);
			}
		}
	}
	HTDestroyOldHT(hash);
	return NewHash;
}

void HTInsert(HashTable hash, void *item, GetKey key){

	(hash->n)++;/*Increase the counter of entries*/
	double LF= ((double) hash->n) / ((double) hash->size); /*Calculate the load factor*/

	int h = Hash(hash, hash->keytype, key(item)); /*Find the index of key*/
	
	List head; /*head points at the linked list we are going to insert the item*/
	if ( (head = hash->chains[h]) == NULL){
		hash->chains[h] = list_create(hash->destroy_item);
		head = hash->chains[h];
	}

	list_insert_next(head, list_last(head), item);

	if(LF >= 0.9){ /*Double the size of the array*/
		hash = Reharshing(hash, key);
	}
}

// void HTRemove(HashTable *phash, KeyType key){
// 	(phash->n)--;/*Decrease the counter of entries*/
// 	int h=Hash(phash, key);/*Find the index of key*/
// 	LList head= *(phash->chains[h]);

// 	bucket SItem=  {
// 		.Key= malloc(sizeof(char)*(strlen(key)+1)),
// 		.Item= 0
// 	};
// 	strcpy(SItem.Key,key);
// 	LLNode S= LLFind(head, &SItem, compare_keys );

// 	if(S!=NULL)
// 		LLRemove(head, S);
// }

void HTPrint(HashTable hash, PrintItem print){

	List head = NULL;

	for (int i = 0; i < hash->size; i++){
		head = hash->chains[i];
		if(head != NULL){
			list_print(head, print);
		}		
	}
}

void HTDestroyOldHT(HashTable hash){
	List head = NULL;

	for (int i = 0; i < hash->size; i++){
		head = hash->chains[i];

		if( head != NULL){
			list_set_destroy_item(head, NULL);
			list_destroy(head);
		}
	}
	free(hash->chains);
	free(hash);
}

void HTDestroy(HashTable hash){
	List head = NULL;

	for (int i = 0; i < hash->size; i++){
		head = hash->chains[i];
			
		if(head != NULL){
			list_destroy(head);
		}
	}
	free(hash->chains);
	free(hash);
}