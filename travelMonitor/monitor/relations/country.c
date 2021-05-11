#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "country.h"

struct  country_struct
{
    char *country_name;
};

country create_country(char *country_name){
    
    country c = (country)malloc(sizeof(struct country_struct));

    c->country_name = (char *)malloc(sizeof(char)*(strlen(country_name)+1));
    strcpy(c->country_name, country_name);

    return c;
}

void *get_country_name(void *c){
    country nc = c;
    return nc->country_name;
}

void print_country(void *c){
    country nc = c;
    printf("%s\n", nc->country_name);
}

int compare_countries(void *key, void *b){
	char *k = key;
	country country_b = b;
	return strcmp(k, country_b->country_name);
}

void destroy_country(void *c){
    country nc = c;
    free(nc->country_name);
    free(nc);
}