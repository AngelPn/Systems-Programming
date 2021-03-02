#ifndef DATE_H
#define DATE_H

#include <stdbool.h>

typedef struct date_struct *date;

date create_date(char *str);

void print_date(date d);

int compare_dates(void *a, void *b);

#endif