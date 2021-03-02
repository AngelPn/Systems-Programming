#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/virus.h"
#include "../include/SkipList.h"

struct virus_struct
{
    char *virusName;
    SkipList vaccinated_persons;
    SkipList not_vaccinated_persons;
};

struct vaccinated_struct
{
    citizenRecord item;
    date dt;
};

vaccinated create_vaccinated(citizenRecord item, date dt){
    vaccinated v = (vaccinated)malloc(sizeof(struct vaccinated_struct));
    v->item = item;
    v->dt = dt;

    return v;
}

void print_vaccinated(void *v){
    vaccinated nv = v;
    print_record(nv->item);
    print_date(nv->dt);
}

void destroy_vaccinated(void *v){
    free(v);
}

virus create_virus(char *virusName){
    virus v = (virus)malloc(sizeof(struct virus_struct));

    v->virusName = (char *)malloc(sizeof(char)*(strlen(virusName)+1));
    strcpy(v->virusName, virusName);

    v->vaccinated_persons = SLCreate(0.5, destroy_vaccinated);
    v->not_vaccinated_persons = SLCreate(0.5, NULL);

    return v;
}

/*Returns pointer to key of virus_struckt*/
void *get_virusName(void *v){
    virus nv = v;
    return nv->virusName; 
}

/*Deallocates memory of virus_struct*/
void destroy_virus(void *v){
    virus nv = v;
    free(nv->virusName);
    SLDestroy(nv->vaccinated_persons);
    SLDestroy(nv->not_vaccinated_persons);
}

/*Prints the fields of record*/
void print_virus_vaccinated_persons(void *v, PrintItem print){
    virus nv = v;
    printf("virusName: %s\n", nv->virusName);
    SLPrint(nv->vaccinated_persons, print);
}

void print_virus_not_vaccinated_persons(void *v, PrintItem print){
    virus nv = v;
    printf("virusName: %s\n", nv->virusName);
    SLPrint(nv->vaccinated_persons, print);
}

/*Compares virusName*/
int compare_virusName(void *key, void *v){
    char *k = key;
    virus nv = v;

    return strcmp(k, nv->virusName);
}