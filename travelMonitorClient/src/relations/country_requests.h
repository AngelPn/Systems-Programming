#ifndef REQUESTS_H
#define REQUESTS_H

#include "List.h"
#include "date.h"

/* "struct country_requests_struct" is incomplete struct */
typedef struct country_requests_struct *country_requests;

/* Creates country_requests_struct */
country_requests create_country_requests(char *countryName);

/* Returns countryName of country_requests_struct */
void *get_country_requests_name(void *c);

/* Returns the list of dates of accepted/rejected requests for country_requests */
List get_accepted(country_requests c);
List get_rejected(country_requests c);

/* Returns the number of accepted/rejected requests between date1 and date2 for country */
int total_country_accepted_requests(country_requests c, date date1, date date2);
int total_country_rejected_requests(country_requests c, date date1, date date2);

/* Compares key with key (countryName) of country_requests v */
int compare_country_requests_name(void *key, void *c);

/* Deallocates memory of country_requests_struct */
void destroy_country_requests(void *c);

#endif