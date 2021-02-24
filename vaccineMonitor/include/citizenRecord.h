#ifndef RECORD_H
#define RECORD_H

#include <stdbool.h>

typedef struct record *citizenRecord;

/*created citizenRecord*/
citizenRecord create_record(
                int citizenID,
                char *firstname,
                char *lastname,
                char *country,
                int age
                );

/*Finds the key of record determined by attrName and returns a pointer to it*/
void *get_key(citizenRecord rec, char *attrName);

/*Deallocates memory of record*/
void destroy_record(void *rec);

/*Prints the fields of record*/
void print_record(void *rec);

#endif /*RECORD_H*/