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
	Entry *ap = a;
	Entry *bp = b;
	return strcmp(ap->Key, bp->Key);
}

int HTGet(HTHash *hash, KeyType key, HTItem *pitem){ /*Function that searches the key in the hash and copies the item in pitem pointer*/
	int h = Hash(hash, key); /*Find the index of key*/
	List head= hash->HashTable[h]; /*head is the linked list of the key we are searching for*/
    //List head= *(hash->HashTable[h]);

	Entry SItem=  {
		.Key= malloc(sizeof(char)*(strlen(key)+1)),
		.Item= 0
	};
	strcpy(SItem.Key,key);
	LLNode P= LLFind(head, &SItem, compare_keys ); /*Pointer P points at the node of the linked list that has the key*/
	
	if(P==NULL) return 0; /*If P is NULL, the key doesn't exist and return false*/
	else {
		Entry *e= LLGetItem(head, P);
		*pitem= e->Item; /*Copy the item*/
		return 1; /*Else, return true*/
	}
}

HTHash *Reharshing(HTHash *phash){
	int m= (phash->size)*2; /*Double the size of the array*/
	HTHash *NewHash= HTCreateHash(m); /*Create a hash with the new size*/

	int i;
	LList *P=NULL;
	KeyType key=NULL;

	for (i=0; i< phash->size; i++){
		P= phash->HashTable[i];
		if(P!= NULL){
			LLNode N= P->dummy->next;
			while(N!=NULL){
				Entry *e= LLGetItem(*P, N);

				key= malloc(sizeof(char)*(strlen(e->Key)+1));
				strcpy(key, e->Key);

				HTItem item= e->Item;

				HTInsert(&NewHash, key, item);
				N= N->next;
			}
		}
	}
	return NewHash;
}

void HTInsert(HTHash **phash, KeyType key, HTItem item){
	HTHash *hash= *phash;
	(hash->n)++;/*Increase the counter of entries*/
	double LF= ((double) hash->n) / ((double) hash->size); /*Calculate the load factor*/
	int h=Hash(hash, key); /*Find the index of key*/
	
	LList *head= hash->HashTable[h]; /*head points at the linked list we are going to insert the item*/
	HTItem pitem;

	Entry NItem=  { /*Create new entry*/
		.Key= malloc(sizeof(char)*(strlen(key)+1)),
		.Item= item
	};
	strcpy(NItem.Key,key);

	if(head!=NULL){
		LList P= *head; /*P is the linked list we are going to insert the item*/
		int exist= HTGet(hash, key, &pitem);
		if(!exist) {
			LLNode node= NULL;
			node= LLInsertAfter(*head, node, &NItem);/*Else, insert the item as first node of the linked list*/
		}
		else{/*If there is the same key*/
			LLNode S= LLFind(*head, &NItem, compare_keys); /*Find the node with the key*/
			Entry *e= LLGetItem(P, S);
			e->Item= item; /*Replace the item*/
		}
	}
	else{
		LList *L= LLCreate(sizeof(Entry)); /*Create the untyped linked list*/
		
		LLNode node= NULL;
		node= LLInsertAfter(*L, node, &NItem);/*Else, insert the item as first node of the linked list*/
		hash->HashTable[h]= L;
	}

	if(LF>= 0.9){ /*Double the size of the array*/
		*phash= Reharshing(hash);
	}
}

void HTRemove(HTHash *phash, KeyType key){
	(phash->n)--;/*Decrease the counter of entries*/
	int h=Hash(phash, key);/*Find the index of key*/
	LList head= *(phash->HashTable[h]);

	Entry SItem=  {
		.Key= malloc(sizeof(char)*(strlen(key)+1)),
		.Item= 0
	};
	strcpy(SItem.Key,key);
	LLNode S= LLFind(head, &SItem, compare_keys );

	if(S!=NULL)
		LLRemove(head, S);
}

void HTVisit(HTHash *hash, HTvisit Function){
	int i;
	LList *P=NULL;
	KeyType key=NULL;

	for (i=0; i< hash->size; i++){

		P= hash->HashTable[i];
		if(P!= NULL){
			LList L= *P;
			LLNode N= L.dummy->next;
			while(N!=NULL){
				Entry *e= LLGetItem(L, N);

				key= malloc(sizeof(char)*strlen(e->Key +1));
				strcpy(key, e->Key); 
				HTItem item= e->Item;
				Function(hash, key, item);
				N= N->next;
			}
		}
	}
}

void HTDestroy(HTHash *phash){
	int i;
	LList *P=NULL;

	for (i=0; i< phash->size; i++){
		P= phash->HashTable[i];
		if(P!=NULL)
			LLDestroy(*P);
	}
	free(phash->HashTable);
	free(phash);
}