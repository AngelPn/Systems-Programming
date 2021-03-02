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
};

struct skiplist_node
{
    int key;
    ListNode lower_level;
};

SLNode create_sl_node(void *key, ListNode lower_level){
    SLNode new = (SLNode)malloc(sizeof(struct skiplist_node));

    new->key = *(int *)key;
    new->lower_level = lower_level;

    return new;
}

void print_sl_node_key(void *node){
    SLNode sl_node = node;
    printf("%d\n", sl_node->key);
}

void destroy_sl_node(void *node){
    SLNode sl_node = node;
    free(sl_node);
}

SkipList SLCreate_with_maxlevel(int max_level, float p, DestroyFunc destroy_item){

    SkipList sl = (SkipList)malloc(sizeof(struct skiplist));

    sl->max_level = max_level;
    sl->p = p;
    sl->level = 0;
    sl->layers = (List *)malloc(sizeof(List)*max_level);

	if(sl->layers == NULL){
		printf("Sorry! System storage is exchausted\n");
		exit(EXIT_FAILURE);
	}

    /*Create an array of lists of size 'max_level': Initialize the Skip Lists with NULL*/
	for(int i = 0; i < max_level; i++)
		sl->layers[i] = NULL;

    sl->layers[0] = list_create(destroy_item);

	return sl;
}

SkipList SLCreate(float p, DestroyFunc destroy_item){
    return SLCreate_with_maxlevel(7, p, destroy_item);
}

int compare_keys(void *key, void *sl_node){
    int k = *(int *)key;
    SLNode node = sl_node;

    if (node->key == k) return 0;
    else if (node->key > k) return -1;
    else return 1;
}

void *SLSearch(SkipList sl, void *key, CompareFunc compare){

    List head = NULL; ListNode node = NULL; SLNode sl_node = NULL;
    CompareFunc compare_function = compare_keys;
    bool found = false;

    for(int i = sl->level; i >= 0; i--){

        /* Compare items in Layer 0 with compare passed by user as argument */
        if (i == 0) compare_function = compare;

        head = sl->layers[i];

        if (node != NULL){
            sl_node = list_node_item(sl->layers[i+1], node);
            node = list_find_order(head, sl_node->lower_level, key, compare_function, &found);            
        }
        else
            node = list_find_order(head, NULL, key, compare_function, &found);       

        if (found == true){
            for (int level = i; level > 0; level--){
                sl_node = list_node_item(sl->layers[level], node);
                node = sl_node->lower_level;
            }
            return list_node_item(head, node);
        }
    }
    return NULL;
}

double my_log(double x, int base) { 
    return log(x) / log(base); 
}

void SLInsert(SkipList sl, void *item, GetKey key, CompareFunc compare, PrintItem print){

    /* Find the path */
    ListNode *path = (ListNode *)malloc(sizeof(ListNode)*(sl->level + 2));

    for (int i = 0; i < sl->level + 2; i++)
        path[i] = NULL;

    List head = NULL;
    CompareFunc compare_function = compare_keys;
    bool found = false;

    // printf("\nSKIP LIST BEFORE\n");
    // SLPrint(sl, print);
    // printf("\n");

    for(int i = sl->level; i >= 0; i--){

        /* Compare items in Layer 0 with compare passed by user as argument */
        if (i == 0) compare_function = compare;

        head = sl->layers[i];
        
        if (path[i+1] != NULL){
            SLNode node = list_node_item(sl->layers[i+1], path[i+1]);
            path[i] = list_find_order(head, node->lower_level, key(item), compare_function, &found);                
        }
        else{
            path[i] = list_find_order(head, NULL, key(item), compare_function, &found); 
        }

        if (found == true){
            printf("The item is already in Skip List\n");
            return ;
        }
    }  

    /* Item must be added in Layer 0 */
    ListNode lower_level = list_insert_next(sl->layers[0], path[0], item);

    /* Flip a coin to decide whether or not to add item in Layer i*/
    for (int i = 1; i < sl->level + 1; i++){
        
        if (rand()%2){
            SLNode new_node = create_sl_node(key(item), lower_level);
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
                SLNode new_node = create_sl_node(&(sl_node->key), node);
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

    printf("LAYER 0\n");
    list_print(sl->layers[0], print);

    List head = NULL;
    for (int i = 1; i < sl->level + 1; i++){
        head = sl->layers[i];

        printf("LAYER %d\n", i);
        list_print(head, print_sl_node_key);   
    }
}

void SLDestroy(SkipList sl){

    for (int i = 0; i < sl->level + 1; i++)
        list_destroy(sl->layers[i]);

    free(sl->layers);
    free(sl);
}