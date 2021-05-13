#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "virus_bloom.h"
#include "country_requests.h"

struct virus_bloom_struct
{
    char *virusName;
    BloomFilter filter;
    List total_accepted_requests;
    List total_rejected_requests;
    HashTable requests_per_country;
};

virus_bloom create_virus_bloom(char *virusName, size_t bytes){
    virus_bloom v = (virus_bloom)malloc(sizeof(struct virus_bloom_struct));

    v->virusName = (char *)malloc(sizeof(char)*(strlen(virusName)+1));
    strcpy(v->virusName, virusName);

    v->filter = BloomCreate(bytes);
    v->total_accepted_requests = list_create(free);
    v->total_rejected_requests = list_create(free);
    v->requests_per_country = HTCreate(String, destroy_country_requests);

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

int total_accepted_requests(virus_bloom v, date date1, date date2){
    int accepted = 0;
    date curr = NULL;
    List head = v->total_accepted_requests;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
        curr = list_node_item(head, node);
        if (date_between(curr, date1, date2)){
            accepted++;
        }
    }
    return accepted;
}

int total_rejected_requests(virus_bloom v, date date1, date date2){
    int rejected = 0;
    date curr = NULL;
    List head = v->total_rejected_requests;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
        curr = list_node_item(head, node);
        if (date_between(curr, date1, date2)){
            rejected++;
        }
    }
    return rejected;
}

int total_accepted_requests_for_country(virus_bloom v, date date1, date date2, char *country){

    country_requests c = NULL;
    if ((c = HTSearch(v->requests_per_country, country, compare_country_requests_name)) == NULL){
        c = create_country_requests(country);
        HTInsert(&(v->requests_per_country), c, get_country_requests_name);
    }

    return total_country_accepted_requests(c, date1, date2);
}

int total_rejected_requests_for_country(virus_bloom v, date date1, date date2, char *country){

    country_requests c = NULL;
    if ((c = HTSearch(v->requests_per_country, country, compare_country_requests_name)) == NULL){
        c = create_country_requests(country);
        HTInsert(&(v->requests_per_country), c, get_country_requests_name);
    }

    return total_country_rejected_requests(c, date1, date2);
}


void insert_date_accepted_requests(virus_bloom v, date dateTravel, char *countryTo){
    list_insert_next(v->total_accepted_requests, NULL, dateTravel);

    country_requests c = NULL;
    if ((c = HTSearch(v->requests_per_country, countryTo, compare_country_requests_name)) == NULL){
        c = create_country_requests(countryTo);
        HTInsert(&(v->requests_per_country), c, get_country_requests_name);
    }

    list_insert_next(get_accepted(c), NULL, dateTravel);
}

void insert_date_rejected_requests(virus_bloom v, date dateTravel, char *countryTo){
    list_insert_next(v->total_rejected_requests, NULL, dateTravel);

    country_requests c = NULL;
    if ((c = HTSearch(v->requests_per_country, countryTo, compare_country_requests_name)) == NULL){
        c = create_country_requests(countryTo);
        HTInsert(&(v->requests_per_country), c, get_country_requests_name);
    }

    list_insert_next(get_rejected(c), NULL, dateTravel);
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
    list_destroy(nv->total_accepted_requests);
    list_destroy(nv->total_rejected_requests);
    HTDestroy(nv->requests_per_country);
    free(nv);
}

