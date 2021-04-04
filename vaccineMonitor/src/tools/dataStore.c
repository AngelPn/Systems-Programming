#include <stdio.h>
#include <stdlib.h>

#include "dataStore.h"
#include "virus.h"
#include "country.h"
#include "citizenRecord.h"

void create_structs(dataStore *ds){
    ds->citizens = HTCreate(Integer, destroy_citizen);
    ds->viruses = HTCreate(String, destroy_virus);
    ds->countries = HTCreate(String, destroy_country);
}

void print_ht_citizens(dataStore *ds){
    HTPrint(ds->citizens, print_citizen);
}

void print_ht_countries(dataStore *ds){
    HTPrint(ds->countries, print_country);
}

void print_ht_viruses(dataStore *ds){
    HTPrint(ds->viruses, print_virus);
}

void destroy_structs(dataStore *ds){
    /* Deallocate memory */
    HTDestroy(ds->countries);
    HTDestroy(ds->citizens);
    HTDestroy(ds->viruses); 
}