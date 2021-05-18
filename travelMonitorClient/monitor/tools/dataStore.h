#ifndef DATA_H
#define DATA_H

#include <stdbool.h>

#include "HashTable.h"
#include "List.h"

typedef struct  data_store
{
    HashTable citizens;
    HashTable countries;
    HashTable viruses;
    List parsed_files;
    int accepted_requests;
    int rejected_requests;

} dataStore;

void create_structs(dataStore *ds);

bool is_parsed(dataStore *ds, char *filename);

void print_ht_citizens(dataStore *ds);
void print_ht_countries(dataStore *ds);
void print_ht_viruses(dataStore *ds);

void destroy_structs(dataStore *ds);


#endif