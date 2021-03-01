#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "../include/SkipList.h"

struct skiplist
{
    int max_level; /* Maximum level for this skip list */
    float p; /* P is the fraction of the nodes with level i pointers also having level i+1 pointers */
    int level; /* Current level of skip list */
    List *layers; /*A Skip List of pointers to untyped linked list*/
    DestroyFunc destroy_item;
};

struct skiplist_node
{
    int key;
    void *item;
    ListNode lower_level;
};

SLNode create_sl_node(void *key, void *item, ListNode lower_level){
    SLNode new = (SLNode)malloc(sizeof(struct skiplist_node));

    new->key = *(int *)key;
    new->item = item;
    new->lower_level = lower_level;

    return new;
}

void print_sl_node(SLNode node, PrintItem print){
    print(node->item);
}

void destroy_sl_node(void *node){
    // destroy_item(node->item); with arguments DestroyFunc
    SLNode sl_node = node;
    free(sl_node);
}

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

int compare_keys(void *key, void *sl_node){
    int k = *(int *)key;
    SLNode node = sl_node;

    if (node->key == k) return 0;
    else if (node->key > k) return -1;
    else return 1;
}

void SLInsert(SkipList sl, void *item, GetKey key, PrintItem print){

    /* Find the path */
    ListNode *path = (ListNode *)malloc(sizeof(ListNode)*(sl->level + 1));

    for (int i = 0; i < sl->level + 1; i++)
        path[i] = NULL;

    List head = NULL;
    bool found = false;

    // printf("\nSKIP LIST BEFORE\n");
    // SLPrint(sl, print);
    // printf("\n");

    if ((head = sl->layers[sl->level]) == NULL){
        sl->layers[sl->level] = list_create(destroy_sl_node);
    }
    else
        path[sl->level] = list_find_order(head, NULL, key(item), compare_keys, &found);

    for(int i = sl->level - 1; i >= 0; i--){

        head = sl->layers[i];
        
        if (path[i+1] != NULL){
            SLNode node = list_node_item(sl->layers[i+1], path[i+1]);
            path[i] = list_find_order(head, node->lower_level, key(item), compare_keys, &found);                
        }
        else{
            path[i] = list_find_order(head, NULL, key(item), compare_keys, &found); 
        }

        if (found == true){
            printf("The item is already in Skip List\n");
            return ;
        }
    }

    /* Item must be added in Layer 0 */
    SLNode new_node = create_sl_node(key(item), item, NULL);
    ListNode lower_level = list_insert_next(sl->layers[0], path[0], new_node);

    /* Flip a coin to decide whether or not to add item in Layer i*/
    for (int i = 1; i < sl->level + 1; i++){
        
        if (rand()%2){
            new_node = create_sl_node(key(item), item, lower_level);
            lower_level = list_insert_next(sl->layers[i], path[i], new_node);
        }

    }
    free(path); 
    
    /* Decide whether or not to add a Layer */
    double result = my_log((double)list_length(sl->layers[0]), (int)1/sl->p);
    if ((int)result > sl->level + 1 && (int)result < sl->max_level){ /* Add Layer */

        (sl->level)++;
        sl->layers[sl->level] = list_create(destroy_sl_node);

        /* Flip a coin to every node of previous Layer to 
        decide whether or not to add the item the new Layer*/
        head = sl->layers[sl->level - 1];
        List head_new = sl->layers[sl->level];
        for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
            if (rand()%2){
                SLNode sl_node = list_node_item(head, node);
                new_node = create_sl_node(&(sl_node->key), sl_node->item, node);
                list_insert_next(head_new, list_last(head_new), new_node);
            }
        }
    }

    // printf("\nSKIP LIST AFTER\n");
    // SLPrint(sl, print);
    // printf("\n");
}

// void SLRemove(SkipList *psl, KeyType key);
void SLPrint(SkipList sl, PrintItem print){
    List head = NULL;

    for (int i = 0; i < sl->level + 1; i++){
        head = sl->layers[i];
        
        printf("LAYER %d\n", i);
        if (head != NULL){
            for (ListNode node = list_first(head); node != NULL; node = list_next(head, node))
                print_sl_node(list_node_item(head, node), print);            
        }

    }
}

void SLDestroy(SkipList sl){

    for (int i = 0; i < sl->level + 1; i++)
        list_destroy(sl->layers[i]);

    free(sl->layers);
    free(sl);
}