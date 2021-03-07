#ifndef DATE_H
#define DATE_H

/* "struct date_struct" is incomplete struct */
typedef struct date_struct *date;

/* Creates date from the string passed as argument */
date create_date(char *str);

/* Creates date with current date */
date current_date();

/* Prints date */
void print_date(date d);

/* Compares two dates */
int compare_dates(void *a, void *b);

#endif