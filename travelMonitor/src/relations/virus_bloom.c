#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "virus_bloom.h"

struct virus_bloom_struct
{
    char *virusName;
    BloomFilter filter;
    List accepted_requests;
    List rejected_requests;
};

virus_bloom create_virus_bloom(char *virusName, size_t bytes){
    virus_bloom v = (virus_bloom)malloc(sizeof(struct virus_bloom_struct));

    v->virusName = (char *)malloc(sizeof(char)*(strlen(virusName)+1));
    strcpy(v->virusName, virusName);

    v->filter = BloomCreate(bytes);
    v->accepted_requests = list_create(free);
    v->rejected_requests = list_create(free);

    return v;
}

void *get_virus_bloomName(void *v){
    virus_bloom nv = v;
    return nv->virusName; 
}

BloomFilter get_bloom(void *v){
    virus_bloom nv = v;
    return nv->filter;    
}

List get_accepted(virus_bloom v){
    return v->accepted_requests;
}

List get_rejected(virus_bloom v){
    return v->rejected_requests;
}

int accepted_requests(virus_bloom v, date date1, date date2){
    int accepted = 0;
    date curr = NULL;
    List head = v->accepted_requests;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
        curr = list_node_item(head, node);
        if (date_between(curr, date1, date2)){
            accepted++;
        }
    }
    return accepted;
}

int rejected_requests(virus_bloom v, date date1, date date2){
    int rejected = 0;
    date curr = NULL;
    List head = v->rejected_requests;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
        curr = list_node_item(head, node);
        if (date_between(curr, date1, date2)){
            rejected++;
        }
    }
    return rejected;
}

void update_BloomFilter(void *v, char *bloom_filter){
    virus_bloom nv = v;
    update_array(nv->filter, bloom_filter);
}

int compare_virus_bloomName(void *key, void *v){
    char *k = key;
    virus_bloom nv = v;

    return strcmp(k, nv->virusName);    
}

void destroy_virus_bloom(void *v){
    virus_bloom nv = v;
    free(nv->virusName);
    BloomDestroy(nv->filter);
    list_destroy(nv->accepted_requests);
    list_destroy(nv->rejected_requests);
    free(nv);
}

