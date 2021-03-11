#ifndef COUNTRY_H
#define COUNTRY_H

/* "struct country_struct" is incomplete struct */
typedef struct country_struct *country;

/* Creates country */
country create_country(char *country_name);

/* Returns a pointer to country's name */
void *get_country_name(void *c);

/* Access fields in country */
int get_vaccinated_persons_num(country c);
int *get_popQueries(country c);

/* Change the values of fields in country */
void increase_vaccinated_persons(country c);
void increase_not_vaccinated_persons(country c);
void increase_popByAge_vaccinated(country c, int age);
void increase_popByAge_not_vaccinated(country c, int age);

/* Reset popQueries to use for another virus */
void reset_popQueries(country c);

/* Print the name of the country */
void print_country(void *c);

/* Compares countries */
int compare_countries(void *key, void *b);

/* Deallocates memory of country */
void destroy_country(void *c);

#endif