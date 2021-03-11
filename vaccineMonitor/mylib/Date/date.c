#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "date.h"

struct date_struct
{
  int day;
  int month;
  int year;
};

/* Convert a string that contains a date to a struct date and return it */
date create_date(char *str){

  date d = (date)malloc(sizeof(struct date_struct));

  if (str == NULL) return NULL;

  char *buf = strdup(str);

  /* Make sure given str is in right format */
  char *token = strtok(buf, "-");
  if (token == NULL) return NULL;
  d->day = atoi(token);
  if (d->day <= 0 || d->day >= 32 ) return NULL;
  
  if ((token = strtok(NULL, "-")) == NULL) return NULL;
  d->month = atoi(token);
  if (d->month <= 0 || d->month >= 13 ) return NULL;

  if ((token = strtok(NULL, "-")) == NULL) return NULL;
  d->year  = atoi(token);
  if (d->year <= 1900) return NULL;

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

bool date_between(date curr_date, date start_date, date end_date){

  if (start_date == NULL || end_date == NULL) /* No date interval has been set, return true*/
    return true;

  else if (compare_dates(curr_date, start_date) >= 0 && compare_dates(end_date, curr_date) >= 0)
    return true;

  else
    return false;
}