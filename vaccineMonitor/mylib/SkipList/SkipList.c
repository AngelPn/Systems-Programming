#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "SkipList.h"

struct skiplist
{
    int max_level;  /* Maximum level for skip list */
    float p;        /* P is the fraction of the nodes with level i pointers also having level i+1 pointers */
    int level;      /* Current number of levels in skip list */
    List *layers;   /*Skip List is array pointers to untyped linked list*/
};

/* Layer i (i > 0) has skiplist_node for item */
struct skiplist_node
{
    int key;
    ListNode lower_level;
};

/* Creates skip list's node for Layer i (i > 0) */
SLNode create_sl_node(void *key, ListNode lower_level){
    SLNode new = (SLNode)malloc(sizeof(struct skiplist_node));

    new->key = *(int *)key;
    printf("new key: %d\n", new->key);
    new->lower_level = lower_level;

    return new;
}

/* Prints key of skip list's node */
void print_sl_node_key(void *node){
    SLNode sl_node = node;
    printf("%d\n", sl_node->key);
}

/* Destroys skiplist_node */
void destroy_sl_node(void *node){
    SLNode sl_node = node;
    free(sl_node);
}

void *get_sl_node_key(void *node){
    printf("HERE-----------------------------------\n");
    SLNode n =  node;
    return &(n->key);
}

int compare_keys(void *key, void *sl_node){
    int k = *(int *)key;
    SLNode node = sl_node;

    if (node->key == k) return 0;
    else if (node->key > k) return -1;
    else return 1;
}

/*-----------------------------------------------------------------------------*/

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

List get_bottom_level(SkipList sl){
    return sl->layers[0];
}

void *SLSearch(SkipList sl, void *key, CompareFunc compare){

    List head = NULL; ListNode node = NULL; SLNode sl_node = NULL;
    bool found = false;

    /* Layer i (i > 0) needs compare_keys and key_function */
    CompareFunc compare_function = compare_keys;
    GetKey key_function = get_sl_node_key;

    /* For each Layer of lists starting with highest level of skip list*/
    for(int i = sl->level; i >= 0; i--){

        /* Layer 0 with compare and key functions passed by user as arguments */
        if (i == 0){
            compare_function = compare;
            key_function = key;
        }

        head = sl->layers[i];

        /*  If node is NULL, search list in Layer i from the start.
            Find the node with given key or
            the previous node of given key in ascending order */
        if (node == NULL)
            node = list_find_order(head, NULL, key_function, compare_function, &found);

        /*  If node is not NULL, search list in Layer i with start_node being the lower_level of node.
            Find the node with given key or
            the previous node of given key in ascending order */       
        else{
            sl_node = list_node_item(sl->layers[i+1], node);
            node = list_find_order(head, sl_node->lower_level, key_function, compare_function, &found);            
        }              

        if (found == true){ /* If found, go to Layer 0 and return node's item */
            for (int level = i; level > 0; level--){
                sl_node = list_node_item(sl->layers[level], node);
                node = sl_node->lower_level;
            }
            return list_node_item(head, node);
        }
    }
    return NULL; /* If not found, return NULL */
}

double my_log(double x, int base) { 
    return log(x) / log(base); 
}


void SLInsert(SkipList sl, void *item, GetKey key, CompareFunc compare){

    /* Find the path */
    ListNode *path = (ListNode *)malloc(sizeof(ListNode)*(sl->level + 2));

    for (int i = 0; i < sl->level + 2; i++)
        path[i] = NULL;

    List head = NULL;
    bool found = false;

    /* Layer i (i > 0) needs compare_keys and key_function */
    CompareFunc compare_function = compare_keys;
    GetKey key_function = get_sl_node_key;
    
    /* For each Layer of lists starting with highest level of skip list*/
    for(int i = sl->level; i >= 0; i--){

        /* Layer 0 with compare and key functions passed by user as arguments */
        if (i == 0){
            compare_function = compare;
            key_function = key;
        }

        head = sl->layers[i];
        
        /* Same process with SLSearch but saves path in path array */
        if (path[i+1] != NULL){
            SLNode node = list_node_item(sl->layers[i+1], path[i+1]);
            path[i] = list_find_order(head, node->lower_level, key_function(item), compare_function, &found);                
        }
        else{
            path[i] = list_find_order(head, NULL, key_function(item), compare_function, &found); 
        }

        if (found == true){
            free(path);
            return ;
        }
    }  

    /* Item must be added in Layer 0 */
    ListNode lower_level = list_insert_next(sl->layers[0], path[0], item);

    /* Flip a coin to decide whether or not to add item in Layer i*/
    for (int i = 1; i < sl->level + 1; i++){
        
        if (rand()%2){
            SLNode new_node = create_sl_node(key_function(item), lower_level);
            lower_level = list_insert_next(sl->layers[i], path[i], new_node);
        }

    }
    free(path); 
    
    /* Decide whether or not to add a Layer */
    double result = my_log((double)list_length(sl->layers[0]), (int)1/sl->p);
    if ((int)result > sl->level + 1 && (int)result < sl->max_level){ /* Add Layer */

        (sl->level)++;

        /* Change GetKey func depending level */
        if ( sl->level > 1){
            printf("here\n");
            key = get_sl_node_key;
        }
            

        sl->layers[sl->level] = list_create(destroy_sl_node);
        printf("level: %d\n", sl->level);
        /* Flip a coin to every node of previous Layer to 
        decide whether or not to add the item in the new Layer*/
        head = sl->layers[sl->level - 1];
        List head_new = sl->layers[sl->level];
        for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
            if (rand()%2){
                SLNode new_node = create_sl_node(key(list_node_item(head, node)), node);
                list_insert_next(head_new, list_last(head_new), new_node);
            }
        }
    }
}


void SLRemove(SkipList sl, void *key, CompareFunc compare){

    List head = NULL; ListNode node = NULL; SLNode sl_node = NULL;
    bool found = false;

    /* Layer i (i > 0) needs compare_keys and key_function */
    CompareFunc compare_function = compare_keys;
    GetKey key_function = get_sl_node_key;

    /* Compare items in Layer i (i > 0) with compare_keys */
    for(int i = sl->level; i >= 0; i--){

        /* Layer 0 with compare and key functions passed by user as arguments */
        if (i == 0){
            compare_function = compare;
            key_function = key;
        }

        head = sl->layers[i];

        /* Same process with SLSearch*/
        if (node == NULL)
            node = list_find_order(head, NULL, key_function, compare_function, &found);   
        else{
            sl_node = list_node_item(sl->layers[i+1], node);
            node = list_find_order(head, sl_node->lower_level, key_function, compare_function, &found);            
        }          

        if (found == true){ /* If found, remove node from every Layer i (i > 0) */
            ListNode lower_node = NULL;
            for (int level = i; level > 0; level--){

                head = sl->layers[level];

                /* Get the node of lower level and store it to lower_node */
                sl_node = list_node_item(head, node);
                lower_node = sl_node->lower_level;

                /* Remove node from list */
                list_remove(head, node);

                /* If list in Layer i (i > 0) occurs empty, destoy list */
                if (list_length(head) == 0 && level > 0){
                    (sl->level)--;
                    list_destroy(head);
                    sl->layers[level] = NULL;
                }

                node = lower_node;
            }
            /* Remove node from Layer 0 */
            list_remove(sl->layers[0], node);
            return;
        }
    }      
}

void SLPrint_BottomLevel(SkipList sl, PrintItem print){ 
    list_print(sl->layers[0], print);
}

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