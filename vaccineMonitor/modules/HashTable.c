/*File: HashTable.c*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/HashTable.h"

HTHash *HTCreateHash(int size){
	HTHash *hash = (HTHash *)malloc(sizeof(HTHash));

	hash->size = size;
	hash->n = 0;
	hash->HashTable = (List *)malloc(sizeof(List)*size);

	if(hash->HashTable == NULL){
		printf("Sorry! System storage is exchausted\n");
		exit(-1);
	}

    /*Create an array of lists of size 'size': Initialize the Hash Table with NULL*/
	for(int i = 0; i < size; i++)
		hash->HashTable[i] = NULL;
	return hash;
}

HTHash *HTCreate(){
	return HTCreateHash(7);
}

int HTSize(HTHash *hash){ /*Function that returns the number of elements in the hash*/
	return hash->n; /*n is the number of entries! */
}

int Hash(HTHash *hash, KeyType key){/*Function that counts the index*/
	int h=0, a=33;
	for(; *key!='\0'; key++)
		h=(a*h + *key)%(hash->size);
	return h;
}

int compare_keys(void *a, void *b) {
	bucket *ap = a;
	bucket *bp = b;
	return strcmp(ap->key, bp->key);
}


/*Function that searches the key in the hash and returns the pointer to this bucket*/
void *HTSearch(HTHash *hash, KeyType key){

	int h = Hash(hash, key); /*Find the index of key*/
	List head = hash->HashTable[h]; /*head is the linked list of the key we are searching for*/
    //List head = *(hash->HashTable[h]);

	bucket searching_node = {
		.key = malloc(sizeof(char)*(strlen(key)+1)),
		.item = NULL
	};
	strcpy(searching_node.key, key);
    pitem = list_find(head, &searching_node, compare_keys);

	free(searching_node.key);

    if (pitem == NULL) return NULL;
    else return pitem;

	// ListNode P= LLFind(head, &SItem, compare_keys ); /*Pointer P points at the node of the linked list that has the key*/
	
	// if(P==NULL) return 0; /*If P is NULL, the key doesn't exist and return false*/
	// else {
	// 	bucket *e= LLGetItem(head, P);
	// 	*pitem= e->Item; /*Copy the item*/
	// 	return 1; /*Else, return true*/
	// }
}

HTHash *Reharshing(HTHash *phash){
	int m = (phash->size)*2; /*Double the size of the array*/
	HTHash *NewHash= HTCreateHash(m); /*Create a hash with the new size*/

	List head = NULL;
	KeyType key = NULL;

	for (int i = 0; i < phash->size; i++){
		head = phash->HashTable[i];
		if(head != NULL){

			ListNode curr_node = head->dummy->next;
			while( curr_node != NULL){
				bucket *e= list_node_item(head, curr_node);

				HTInsert(&NewHash, e->key, e->item);
				curr_node = curr_node->next;
			}
		}
	}
	HTDestroy(phash);
	return NewHash;
}

void destroy_bucket(void *b){
	bucket *nb = b;
	free(nb->key);
}

void HTInsert(HTHash **phash, KeyType key, void *item){
	HTHash *hash= *phash;

	(hash->n)++;/*Increase the counter of entries*/
	double LF= ((double) hash->n) / ((double) hash->size); /*Calculate the load factor*/

	int h = Hash(hash, key); /*Find the index of key*/
	
	List head; /*head points at the linked list we are going to insert the item*/
	if ( (head = hash->HashTable[h]) == NULL){
		hash->HashTable[h] = list_create(destroy_bucket);
		head = hash->HashTable[h];
	}

	bucket new_bucket = { /*Create new bucket*/
		.key = malloc(sizeof(char)*(strlen(key)+1)),
		.item = item
	};
	strcpy(new_bucket.key, key);

	list_insert_next(head, list_last(head), &new_bucket);

	if(LF >= 0.9){ /*Double the size of the array*/
		*phash= Reharshing(hash);
	}
}

void HTRemove(HTHash *phash, KeyType key){
	(phash->n)--;/*Decrease the counter of entries*/
	int h=Hash(phash, key);/*Find the index of key*/
	LList head= *(phash->HashTable[h]);

	bucket SItem=  {
		.Key= malloc(sizeof(char)*(strlen(key)+1)),
		.Item= 0
	};
	strcpy(SItem.Key,key);
	LLNode S= LLFind(head, &SItem, compare_keys );

	if(S!=NULL)
		LLRemove(head, S);
}

// void HTVisit(HTHash *hash, HTvisit Function){
// 	int i;
// 	LList *P=NULL;
// 	KeyType key=NULL;

// 	for (i=0; i< hash->size; i++){

// 		P= hash->HashTable[i];
// 		if(P!= NULL){
// 			LList L= *P;
// 			LLNode N= L.dummy->next;
// 			while(N!=NULL){
// 				bucket *e= LLGetItem(L, N);

// 				key= malloc(sizeof(char)*strlen(e->Key +1));
// 				strcpy(key, e->Key); 
// 				void item= e->Item;
// 				Function(hash, key, item);
// 				N= N->next;
// 			}
// 		}
// 	}
// }

void HTDestroy(HTHash *phash){
	List P = NULL;

	for (int i = 0; i < phash->size; i++){
		P = phash->HashTable[i];
		if( P != NULL)
			list_destroy(P);
	}
	free(phash->HashTable);
	free(phash);
}