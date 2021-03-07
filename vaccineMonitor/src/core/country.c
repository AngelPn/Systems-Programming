#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "country.h"

struct  country_struct
{
    char *country_name;
    int population;
};

country create_country(char *country_name){
    
    country c = (country)malloc(sizeof(struct country_struct));

    c->country_name = (char *)malloc(sizeof(char)*(strlen(country_name)+1));
    strcpy(c->country_name, country_name);

    c->population = 1;

    return c;
}

char *get_country_name(country c){
    c->country_name;
}

int get_population(country c){
    return c->population;
}

void increase_population(country c){
    (c->population)++;
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