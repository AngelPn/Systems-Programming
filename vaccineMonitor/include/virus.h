#ifndef VIRUS_H
#define VIRUS_H

#include "common_types.h"
#include "date.h"
#include "citizenRecord.h"

typedef struct virus_struct *virus;
typedef struct vaccinated_struct *vaccinated;

vaccinated create_vaccinated(citizenRecord item, date dt);

void destroy_vaccinated(void *vaccinated);

/*create virus_struct*/
virus create_virus(char *virusName);

/*Returns pointer to key of virus_struckt*/
void *get_virusName(void *virus);

/*Deallocates memory of virus_struct*/
void destroy_virus(void *virus);

/*Prints the fields of record*/
void print_virus(void *virus);

/*Compares virusName*/
int compare_virusName(void *key, void *virus);

#endif