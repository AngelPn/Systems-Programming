#ifndef COUNTRY_H
#define COUNTRY_H

/* "struct country_struct" is incomplete struct */
typedef struct country_struct *country;

/* Creates citizenRecord */
country create_country(char *country_name);

/* Returns a pointer to country's name */
void *get_country_name(void *c);

int get_population(country c);

void increase_population(country c);

void increase_vaccinated_persons(country c);
void reset_vaccinated_persons(country c);

/* Compares countries */
int compare_countries(void *key, void *b);

/* Deallocates memory of record */
void destroy_country(void *c);

#endif