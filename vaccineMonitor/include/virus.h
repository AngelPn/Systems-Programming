#ifndef VIRUS_H
#define VIRUS_H

#include "common_types.h"
#include "date.h"
#include "citizenRecord.h"
#include "SkipList.h"
#include "BloomFilter.h"

/* "struct vaccinated_struct" is incomplete struct */
typedef struct vaccinated_struct *vaccinated;

/* Creates vaccinated */
vaccinated create_vaccinated(citizenRecord item, date dt);

/* Finds the key of vaccinated_struct and returns a pointer to it */
void *get_vaccinated_key(void *v);

/* Prints vaccinated citizen's informations */
void print_vaccinated(void *v);

/* Prints date of vaccination */
void print_vaccinated_date(vaccinated v);

/* Compares key with vaccinated citizen's ID */
int compare_vaccinated(void *key, void *v);

/* Deallocates memory of vaccinated citizen */
void destroy_vaccinated(void *vaccinated);

/*---------------------------------------------------------------*/

/* "struct virus_struct" is incomplete struct */
typedef struct virus_struct *virus;

/* Creates virus_struct */
virus create_virus(char *virusName, size_t kilobytes);

/* Returns pointer to virusName of virus_struct */
void *get_virusName(void *v);

/* Returns bloom filter of virus */
BloomFilter get_filter(void *v);

/* Returns skip lists of virus */
SkipList get_vaccinated_persons(void *v);
SkipList get_not_vaccinated_persons(void *v);

/*Prints the fields of record*/
void print_virus(void *v);

/* Compares key with virusName of virus v */
int compare_virusName(void *key, void *v);

/* Deallocates memory of virus_struct */
void destroy_virus(void *v);

#endif