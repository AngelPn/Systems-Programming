#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dataStore.h"
#include "virus.h"
#include "country.h"
#include "citizenRecord.h"

void create_structs(dataStore *ds){
    ds->citizens = HTCreate(Integer, destroy_citizen);
    ds->viruses = HTCreate(String, destroy_virus);
    ds->countries = HTCreate(String, destroy_country);
    ds->parsed_files = list_create(free);
}

int compare_filename(void *filename1, void *filename2){
    return strcmp((char *)filename1, (char *)filename2);    
}

bool is_parsed(dataStore *ds, char *filename){
    if (list_find(ds->parsed_files, filename, compare_filename) != NULL)
        return true;
    else return false;
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
    list_destroy(ds->parsed_files);
}