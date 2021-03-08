#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "virus.h"

struct vaccinated_struct
{
    citizenRecord item;
    date dateVaccinated;
};

vaccinated create_vaccinated(citizenRecord item, date dateVaccinated){
    vaccinated v = (vaccinated)malloc(sizeof(struct vaccinated_struct));
    v->item = item;
    v->dateVaccinated = dateVaccinated;

    return v;
}

void *get_vaccinated_key(void *v){
    vaccinated nv = v;
    return get_citizenID(nv->item);
}

date get_vaccinated_date(vaccinated v){
    return v->dateVaccinated;
}

citizenRecord get_citizen(vaccinated v){
    return v->item;
}

void print_vaccinated_date(vaccinated v){
    return print_date(v->dateVaccinated);
}

int compare_vaccinated(void *key, void *v){
    vaccinated nv = v;
    return compare_citizen(key, nv->item);
}

void print_vaccinated(void *v){
    vaccinated nv = v;
    print_citizen(nv->item);
    print_date(nv->dateVaccinated);
}

void destroy_vaccinated(void *v){
    vaccinated nv = v;
    free(nv->dateVaccinated);
    free(v);
}

/*-------------------------------------------------------------------*/

struct virus_struct
{
    char *virusName;
    BloomFilter filter;
    SkipList vaccinated_persons;
    SkipList not_vaccinated_persons;
};

virus create_virus(char *virusName, size_t kilobytes){
    virus v = (virus)malloc(sizeof(struct virus_struct));

    v->virusName = (char *)malloc(sizeof(char)*(strlen(virusName)+1));
    strcpy(v->virusName, virusName);

    v->filter = BloomCreate(kilobytes);
    v->vaccinated_persons = SLCreate(0.5, destroy_vaccinated);
    v->not_vaccinated_persons = SLCreate(0.5, NULL);

    return v;
}

/*Returns pointer to key of virus_struct*/
void *get_virusName(void *v){
    virus nv = v;
    return nv->virusName; 
}

BloomFilter get_filter(void *v){
    virus nv = v;
    return nv->filter;
}

SkipList get_vaccinated_persons(void *v){
    virus nv = v;
    return nv->vaccinated_persons;
}

SkipList get_not_vaccinated_persons(void *v){
    virus nv = v;
    return nv->not_vaccinated_persons;
}

/*Prints the fields of record*/
void print_virus(void *v){
    virus nv = v;
    printf("\n-----------------------------------------------------------");
    printf("\nvirusName: %s\n", nv->virusName);

    printf("\nvaccinated persons:\n");
    SLPrint(nv->vaccinated_persons, print_vaccinated);

    printf("\nnot vaccinated persons:\n");
    SLPrint(nv->not_vaccinated_persons, print_citizen);
}

/*Compares virusName*/
int compare_virusName(void *key, void *v){
    char *k = key;
    virus nv = v;

    return strcmp(k, nv->virusName);
}

/*Deallocates memory of virus_struct*/
void destroy_virus(void *v){
    virus nv = v;
    free(nv->virusName);
    BloomDestroy(nv->filter);
    SLDestroy(nv->vaccinated_persons);
    SLDestroy(nv->not_vaccinated_persons);
    free(nv);
}