#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "country_requests.h"

struct country_requests_struct
{
    char *countryName;
    List accepted_requests;
    List rejected_requests;
};

country_requests create_country_requests(char *countryName){
    country_requests c = (country_requests)malloc(sizeof(struct country_requests_struct));

    c->countryName = (char *)malloc(sizeof(char)*(strlen(countryName)+1));
    strcpy(c->countryName, countryName);

    c->accepted_requests = list_create(NULL);
    c->rejected_requests = list_create(NULL);

    return c;
}

void *get_country_requests_name(void *c){
    country_requests nc = c;
    return nc->countryName; 
}

List get_accepted(country_requests c){
    return c->accepted_requests;
}

List get_rejected(country_requests c){
    return c->rejected_requests;
}

int total_country_accepted_requests(country_requests c, date date1, date date2){
    int accepted = 0;
    date curr = NULL;
    List head = c->accepted_requests;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
        curr = list_node_item(head, node);
        if (date_between(curr, date1, date2)){
            accepted++;
        }
    }
    return accepted;
}

int total_country_rejected_requests(country_requests c, date date1, date date2){
    int rejected = 0;
    date curr = NULL;
    List head = c->rejected_requests;
    for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
        curr = list_node_item(head, node);
        if (date_between(curr, date1, date2)){
            rejected++;
        }
    }
    return rejected;
}

int compare_country_requests_name(void *key, void *c){
    char *k = key;
    country_requests nc = c;

    return strcmp(k, nc->countryName);    
}

void destroy_country_requests(void *c){
    country_requests nc = c;
    free(nc->countryName);
    list_destroy(nc->accepted_requests);
    list_destroy(nc->rejected_requests);
    free(nc);
}