#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/date.h"

typedef struct date_struct
{
  int day;
  int month;
  int year;
};

/* Convert a string that contains a date to a struct date and return it */
date create_date(char *str){

  date d = (date)malloc(sizeof(struct date_struct));

  char *buf = strdup(str);
  char *token = strtok(buf, "-");

  d->day = atoi(token);
  d->month = atoi(strtok(NULL, "-"));
  d->year  = atoi(strtok(NULL, "-"));

  free(buf);

  return d;
}

date current_date(){

  date d = (date)malloc(sizeof(struct date_struct));

  time_t now; /* time_t is arithmetic time type */
 
  /* Obtain current time */
  time(&now);

  /* localtime converts a time_t value to calendar time and
     returns a pointer to a tm structure with its members
     filled with the corresponding values */
  struct tm *local = localtime(&now);

  d->day = local->tm_mday;            /* get day of month (1 to 31) */
  d->month = local->tm_mon + 1;       /* get month of year (0 to 11) */
  d->year = local->tm_year + 1900;    /* get year since 1900 */

  return d;
}

void print_date(date d){
    printf("%02d-%02d-%4d\n", d->day, d->month, d->year);
}

// Returns -1 if a < b
// Returns  0 if a == b
// Returns  1 if a > b
int compare_dates(void *a, void *b){
  date d1 = a, d2 = b;

  if (d1->year < d2->year)
    return -1;
  if (d1->year > d2->year)
    return 1;
  if (d1->month < d2->month)
    return -1;
  if (d1->month > d2->month)
    return 1;
  if (d1->day < d2->day)
    return -1;
  if (d1->day > d2->day)
    return 1;

  return 0;
}