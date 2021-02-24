#ifndef DATE_H
#define DATE_H

#include <stdbool.h>

typedef struct date_struct{
  int day;
  int month;
  int year;
} date;

void print_date(date d);

void convert_str_to_date(char *str, date *d);

int compare_dates(void *a, void *b);

#endif