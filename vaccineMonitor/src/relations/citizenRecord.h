#ifndef RECORD_H
#define RECORD_H

#include "country.h"

/* "struct record" is incomplete struct */
typedef struct record *citizenRecord;

/* Creates citizenRecord */
citizenRecord create_citizen(int citizenID, char *firstname, char *lastname, country cntry, int age);

/* Finds the key of citizen's record and returns a pointer to it */
void *get_citizenID(void *rec);

country get_country(citizenRecord rec);

int get_age(citizenRecord rec);

/* Prints the fields of record */
void print_citizen(void *rec);

/* Compares citizenIDs */
int compare_citizen(void *key, void *rec);

/* Deallocates memory of record */
void destroy_citizen(void *rec);

#endif