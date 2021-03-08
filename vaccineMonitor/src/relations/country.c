#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "country.h"

struct  country_struct
{
    char *country_name;
    int population;
    int *popByAge;
};

country create_country(char *country_name){
    
    country c = (country)malloc(sizeof(struct country_struct));

    c->country_name = (char *)malloc(sizeof(char)*(strlen(country_name)+1));
    strcpy(c->country_name, country_name);

    c->population = 1;
    c->popByAge = NULL;

    return c;
}

void *get_country_name(void *c){
    country nc = c;
    return nc->country_name;
}

int get_population(country c){
    return c->population;
}

int get_vaccinated_persons_num(country c){
    if (c->popByAge != NULL)
        return c->popByAge[0];
    else return 0;
}

int *get_popByAge(country c){
    return c->popByAge;
}

void increase_population(country c){
    (c->population)++;
}

void increase_vaccinated_persons(country c){

    if (c->popByAge == NULL){
        c->popByAge = (int *)malloc(sizeof(int)*1);
        c->popByAge[0] = 0;
    }      
    (c->popByAge[0])++;
}

void increase_popByAge(country c, int age){

    if (c->popByAge == NULL){
        c->popByAge = (int *)malloc(sizeof(int)*4);

        for (int i = 0; i < 4; i++)
            c->popByAge[i] = 0;
    }

    if (age < 20)
        (c->popByAge[0])++;
    else if (age >= 20 && age < 40)
        (c->popByAge[1])++;
    else if (age >= 40 && age < 60)
        (c->popByAge[2])++;
    else
        (c->popByAge[3])++;
}

void reset_vaccinated_persons(country c){
    free(c->popByAge);
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