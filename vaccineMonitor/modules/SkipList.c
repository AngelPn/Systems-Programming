#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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
		sl->layers[i] = NULL;
	return sl;
}

SkipList SLCreate(float p, DestroyFunc destroy_item){
    return SLCreate_with_maxlevel(7, p, destroy_item);
}

void *SLSearch(SkipList sl, void *key, CompareFunc compare){

    List head; ListNode node, lower_node = NULL;
    bool found = false;
    for(int i = sl->level; i < 0; i--){

        head = sl->layers[i];
        node = list_find_order(head, lower_node, key, compare, &found);
        

        if (found == true)
            return list_node_item(head, node);

        lower_node = node;     
    }
    return NULL;
}

double my_log(double x, int base) { 
    return log(x) / log(base); 
} 


void SLInsert(SkipList sl, void *item, GetKey key, CompareFunc compare){

    /* Find the path */
    ListNode *path = (ListNode *)malloc(sizeof(ListNode)*(sl->level + 1));

    for (int i = 0; i < sl->level + 1; i++)
        path[i] = NULL;

    List head = NULL;
    bool found = false;
    for(int i = sl->level; i < 0; i--){
        printf("i = %d\n", i);
        if ((head = sl->layers[i]) == NULL){
            sl->layers[i] = list_create(NULL);
            break;
        }
        path[i] = list_find_order(head, path[i-1], key(item), compare, &found);

        if (found == true){
            printf("The item is already in Skip List\n");
            return ;
        }
    }

    /* Item must be added in Layer 0 */
    list_insert_next(sl->layers[0], path[0], item);

    /* Flip a coin to decide whether or not to add item in Layer i*/
    for (int i = 1; i < sl->level + 1; i++){
        
        if (rand()%2){
            list_insert_next(sl->layers[i], path[i], item);
        }
    }
    free(path); 
    
    /* Decide whether or not to add a Layer */
    double result = my_log((double)list_length(sl->layers[0]), (int)1/sl->p);
    printf("result = %f\n", result);
    if ((int)result > sl->level + 1 && (int)result < sl->max_level){ /* Add Layer */

        (sl->level)++;
        sl->layers[sl->level] = list_create(NULL);

        /* Flip a coin to every node of previous Layer to 
        decide whether or not to add the item the new Layer*/
        head = sl->layers[sl->level - 1];
        for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
            if (rand()%2){
                list_insert_next(sl->layers[sl->level], list_last(sl->layers[sl->level]), item);
            }
        }
    }
}

// void SLRemove(SkipList *psl, KeyType key);
void SLPrint(SkipList sl, PrintItem print);

void SLDestroy(SkipList sl){

    for (int i = 0; i < sl->level + 1; i++)
        list_destroy(sl->layers[i]);

    free(sl->layers);
    free(sl);
}