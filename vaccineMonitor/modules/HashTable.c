/*File: HashTable.c*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/HashTable.h"

bucket create_bucket(KeyType key, void *item){
	bucket b = (bucket)malloc(sizeof(struct entry));

	b->key = (KeyType)malloc(sizeof(char)*(1+strlen(key)));
	b->item = item;

	return b;
}

HashTable HTCreateHash(int size){
	HashTable hash = (HashTable)malloc(sizeof(struct hashtable));

	hash->size = size;
	hash->n = 0;
	hash->chains = (List *)malloc(sizeof(List)*size);

	if(hash->chains == NULL){
		printf("Sorry! System storage is exchausted\n");
		exit(-1);
	}

    /*Create an array of lists of size 'size': Initialize the Hash Table with NULL*/
	for(int i = 0; i < size; i++)
		hash->chains[i] = NULL;
	return hash;
}

HashTable HTCreate(){
	return HTCreateHash(7);
}

int HTSize(HashTable hash){ /*Function that returns the number of elements in the hash*/
	return hash->n; /*n is the number of entries! */
}

int Hash(HashTable hash, KeyType key){/*Function that counts the index*/
	int h=0, a=33;
	for(; *key!='\0'; key++)
		h=(a*h + *key)%(hash->size);
	return h;
}

int compare_keys(void *a, void *b) {
	bucket ap = a;
	bucket bp = b;
	return strcmp(ap->key, bp->key);
}


/*Function that searches the key in the hash and returns the pointer to this bucket*/
void *HTSearch(HashTable hash, KeyType key){

	int h = Hash(hash, key); /*Find the index of key*/
	List head = hash->chains[h]; /*head is the linked list of the key we are searching for*/
    //List head = *(hash->chains[h]);

	if (head != NULL){
		bucket searching_node = create_bucket(key, NULL);

		void *pitem = list_find(head, searching_node, compare_keys);

		free(searching_node->key);

		if (pitem == NULL) return NULL;
		else return pitem;		
	}
	else return NULL;

}

HashTable Reharshing(HashTable phash){

	HashTable NewHash= HTCreateHash((phash->size)*2); /*Create a hash with the new size*/

	List head = NULL;

	for (int i = 0; i < phash->size; i++){
		head = phash->chains[i];
		if(head != NULL){

			ListNode curr_node = list_first(head);
			while( curr_node != NULL){
				bucket e= list_node_item(head, curr_node);

				HTInsert(NewHash, e->key, e->item);
				curr_node = list_next(head, curr_node);
			}
		}
	}
	HTDestroy(phash);
	return NewHash;
}

void destroy_bucket(void *b){
	bucket nb = b;
	free(nb->key);
	free(nb);
}

void HTInsert(HashTable hash, KeyType key, void *item){

	(hash->n)++;/*Increase the counter of entries*/
	double LF= ((double) hash->n) / ((double) hash->size); /*Calculate the load factor*/

	int h = Hash(hash, key); /*Find the index of key*/
	printf("index %d\n", h);
	
	List head; /*head points at the linked list we are going to insert the item*/
	if ( (head = hash->chains[h]) == NULL){
		hash->chains[h] = list_create(destroy_bucket);
		head = hash->chains[h];
	}

	bucket new_bucket = create_bucket(key, item);

	list_insert_next(head, list_last(head), new_bucket);

	if(LF >= 0.9){ /*Double the size of the array*/
		hash = Reharshing(hash);
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

			ListNode curr_node = list_first(head);
			while( curr_node != NULL){
				bucket e= list_node_item(head, curr_node);

				printf("Key: %s\n", e->key);
				// print(e->item);
				curr_node = list_next(head, curr_node);
			}
		}		
	}
}

// void HTVisit(HashTable *hash, HTvisit Function){
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

void HTDestroy(HashTable phash){
	List P = NULL;

	for (int i = 0; i < phash->size; i++){
		P = phash->chains[i];
		if( P != NULL)
			list_destroy(P);
	}
	free(phash->chains);
	free(phash);
}