#ifndef COUNTRY_H
#define COUNTRY_H

/* "struct country_struct" is incomplete struct */
typedef struct country_struct *country;

/* Creates country */
country create_country(char *country_name);

/* Returns a pointer to country's name */
void *get_country_name(void *c);

/* Change the values of population query in country */
void increase_vaccinated_persons(country c);
void increase_not_vaccinated_persons(country c);
void increase_popByAge_vaccinated(country c, int age);
void increase_popByAge_not_vaccinated(country c, int age);

/* Prints data for population queries */
void populationStatus(void *item, int key);
void popStatusByAge(void *item, int key);

/* Print the name of the country */
void print_country(void *c);

/* Compares countries */
int compare_countries(void *key, void *b);

/* Deallocates memory of country */
void destroy_country(void *c);

#endif