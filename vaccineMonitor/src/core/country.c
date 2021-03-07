#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "country.h"

struct  country_struct
{
    char *country_name;
    int population;
    int vaccinated_persons;
};

country create_country(char *country_name){
    
    country c = (country)malloc(sizeof(struct country_struct));

    c->country_name = (char *)malloc(sizeof(char)*(strlen(country_name)+1));
    strcpy(c->country_name, country_name);

    c->population = 1;
    c->vaccinated_persons = 0;

    return c;
}

void *get_country_name(void *c){
    country nc = c;
    return nc->country_name;
}

int get_population(country c){
    return c->population;
}

void increase_population(country c){
    (c->population)++;
}

void increase_vaccinated_persons(country c){
    (c->vaccinated_persons)++;
}

void reset_vaccinated_persons(country c){
    c->vaccinated_persons = 0;
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