#ifndef RECORD_H
#define RECORD_H

/* "struct record" is incomplete struct */
typedef struct record *citizenRecord;

/* Creates citizenRecord */
citizenRecord create_citizen(int citizenID, char *firstname, char *lastname, char *country, int age);

/* Finds the key of citizen's record and returns a pointer to it */
void *get_citizenID(void *rec);

/* Prints the fields of record */
void print_citizen(void *rec);

/* Compares citizenIDs */
int compare_citizen(void *key, void *rec);

/* Deallocates memory of record */
void destroy_citizen(void *rec);

#endif