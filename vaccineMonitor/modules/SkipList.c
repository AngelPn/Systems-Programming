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

SkipList SLCreate(float p, DestroyFunc destroy_item){
    SkipList sl = (SkipList)malloc(sizeof(struct skiplist));

    sl->p = p;
    
}

void *SLSearch(SkipList sl, void *key, CompareFunc compare);
void SLInsert(SkipList sl, void *item, GetKey key);
// void SLRemove(SkipList *psl, KeyType key);
void SLPrint(SkipList sl, PrintItem print);
void SLDestroy(SkipList sl);