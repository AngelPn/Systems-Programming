#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "country.h"

struct  country_struct
{
    char *country_name;
    int *popQuery;      /* used for population queries */
};

country create_country(char *country_name){
    
    country c = (country)malloc(sizeof(struct country_struct));

    c->country_name = (char *)malloc(sizeof(char)*(strlen(country_name)+1));
    strcpy(c->country_name, country_name);

    c->popQuery = NULL;

    return c;
}

void *get_country_name(void *c){
    country nc = c;
    return nc->country_name;
}

int get_vaccinated_persons_num(country c){
    if (c->popQuery != NULL)
        return c->popQuery[0];
    else return 0;
}

int *get_popQueries(country c){
    return c->popQuery;
}

/*  If query is /populationStatus, 
    then popQuery[0] keeps the number of vaccinated persons for certain virus
    and popQuery[1] keeps the number of not vaccinated persons for the same virus */
void increase_vaccinated_persons(country c){

    if (c->popQuery == NULL){
        c->popQuery = (int *)malloc(sizeof(int)*2);
        c->popQuery[0] = 0;
        c->popQuery[1] = 0;
    }      
    (c->popQuery[0])++;
}

void increase_not_vaccinated_persons(country c){

    if (c->popQuery == NULL){
        c->popQuery = (int *)malloc(sizeof(int)*2);
        c->popQuery[0] = 0;
        c->popQuery[1] = 0;
    }      
    (c->popQuery[1])++;
}

/*  If query is /popStatusByAge, 
    then popQuery[0-4] keeps the number of vaccinated persons by age for certain virus
    and popQuery[4-7] keeps the number of not vaccinated persons by age for the same virus */
void increase_popByAge_vaccinated(country c, int age){

    if (c->popQuery == NULL){
        c->popQuery = (int *)malloc(sizeof(int)*8);

        for (int i = 0; i < 8; i++)
            c->popQuery[i] = 0;
    }

    if (age < 20)
        (c->popQuery[0])++;
    else if (age >= 20 && age < 40)
        (c->popQuery[1])++;
    else if (age >= 40 && age < 60)
        (c->popQuery[2])++;
    else
        (c->popQuery[3])++;
}

void increase_popByAge_not_vaccinated(country c, int age){

    if (c->popQuery == NULL){
        c->popQuery = (int *)malloc(sizeof(int)*8);

        for (int i = 0; i < 8; i++)
            c->popQuery[i] = 0;
    }

    if (age < 20)
        (c->popQuery[4])++;
    else if (age >= 20 && age < 40)
        (c->popQuery[5])++;
    else if (age >= 40 && age < 60)
        (c->popQuery[6])++;
    else
        (c->popQuery[7])++;
}

/* Reset popQueries to use for another virus */
void reset_popQueries(country c){
    free(c->popQuery);
    c->popQuery = NULL;
}

void populationStatus(void *item, int key){

	country c = item;

	if (c->popQuery != NULL){
		printf("%s %d %.2f%%\n", c->country_name, c->popQuery[0], (double)c->popQuery[0]/(double)(c->popQuery[0]+c->popQuery[1])*100);
		reset_popQueries(c);
	}
}

void popStatusByAge(void *item, int key){

	country c = item;

	if (c->popQuery != NULL){
		printf("\n%s\n", c->country_name);
		printf("0-20 %d %.2f%%\n", c->popQuery[0], (double)c->popQuery[0]/(double)(c->popQuery[0]+c->popQuery[4])*100);
		printf("20-40 %d %.2f%%\n", c->popQuery[1], (double)c->popQuery[1]/(double)(c->popQuery[1]+c->popQuery[5])*100);
		printf("40-60 %d %.2f%%\n", c->popQuery[2], (double)c->popQuery[2]/(double)(c->popQuery[2]+c->popQuery[6])*100);
		printf("60+ %d %.2f%%\n", c->popQuery[3], (double)c->popQuery[3]/(double)(c->popQuery[3]+c->popQuery[7])*100);
		reset_popQueries(c);
	}
	else{
		printf("\nNobody in country %s is vaccinated for this virus\n", (char *)get_country_name(c));
	}
}

void print_country(void *c){
    country nc = c;
    printf("%s", nc->country_name);
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