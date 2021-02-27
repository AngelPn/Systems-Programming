#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/SkipList.h"

struct skiplist{
    int max_level; /* Maximum level for this skip list */
    float p; /* P is the fraction of the nodes with level i pointers also having level i+1 pointers */
    int level; /* Current level of skip list */
    List *layers; /*A Skip List of pointers to untyped linked list*/
    DestroyFunc destroy_item;
};

struct skiplist_node{
    //int key;
    void *item;
    ListNode lower_level;
};

SkipList SLCreate_with_maxlevel(int max_level, float p, DestroyFunc destroy_item){
    SkipList sl = (SkipList)malloc(sizeof(struct skiplist));

    sl->max_level = max_level;
    sl->p = p;
    sl->level = 0;
    sl->layers = (List *)malloc(sizeof(List)*max_level);
    sl->destroy_item = destroy_item;

	if(sl->layers == NULL){
		printf("Sorry! System storage is exchausted\n");
		exit(EXIT_FAILURE);
	}

    /*Create an array of lists of size 'max_level': Initialize the Skip Lists with NULL*/
	for(int i = 0; i < max_level; i++)
		sl->layers[max_level] = NULL;
	return sl;
}

SkipList SLCreate(float p, DestroyFunc destroy_item){
    return SLCreate_with_maxlevel(7, p, destroy_item);
}

void *SLSearch(SkipList sl, void *key, CompareFunc compare){

    ListNode *path = (ListNode *)malloc(sizeof(ListNode)*(sl->level + 1));

    for (int i = 0; i < sl->level + 1; i++)
        path[i] = NULL;

    List head;
    for(int i = sl->level; i = 0; i--){
        head = sl->layers[i];
        path[i] = list_find_order(head, path[i-1], key, compare);
    }

}

void SLInsert(SkipList sl, void *item, GetKey key){

    List head = NULL;
    /*head points at the linked list of Layer 0*/
    if ( (head = sl->layers[0]) == NULL){
        sl->layers[0] = list_create(NULL);
        head = sl->layers[0];
    }


}

// void SLRemove(SkipList *psl, KeyType key);
void SLPrint(SkipList sl, PrintItem print);
void SLDestroy(SkipList sl);