#ifndef DATE_H
#define DATE_H

#include <stdbool.h>

/* "struct date_struct" is incomplete struct */
typedef struct date_struct *date;

/* Creates date from the string passed as argument */
date create_date(char *str);

/* Creates date with current date */
date current_date();

/* Returns date as string */
char *get_date_as_str(date d);

/* Prints date */
void print_date(date d);

/* Compares two dates */
int compare_dates(void *a, void *b);

/* Returns true if curr_date is between begin_date and end_date */
bool date_between(date curr_date, date begin_date, date end_date);

#endif