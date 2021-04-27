#ifndef DATA_H
#define DATA_H

#include "HashTable.h"
#include "List.h"

typedef struct  data_store
{
    HashTable citizens;
    HashTable countries;
    HashTable viruses;
    List parsed_files;

} dataStore;

void create_structs(dataStore *ds);

void print_ht_citizens(dataStore *ds);
void print_ht_countries(dataStore *ds);
void print_ht_viruses(dataStore *ds);

void destroy_structs(dataStore *ds);


#endif