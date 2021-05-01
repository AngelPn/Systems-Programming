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

  if (str == NULL) return NULL;

  char *buf = strdup(str), *token;
  int dd, mm, yy;

  /* Make sure given str is in right format */
  if ((token = strtok(buf, "-")) == NULL){
    free(buf);
    return NULL;
  }
  dd = atoi(token);
  if (dd <= 0 || dd >= 32 ){
    free(buf);
    return NULL;    
  }
  
  if ((token = strtok(NULL, "-")) == NULL){
    free(buf);
    return NULL;
  }
  mm = atoi(token);
  if (mm <= 0 || mm >= 13 ){
    free(buf);
    return NULL;
  }

  if ((token = strtok(NULL, "-")) == NULL){
    free(buf);
    return NULL;
  }
  yy  = atoi(token);
  if (yy <= 1900){
    free(buf);
    return NULL;
  }

  free(buf);

  date d = (date)malloc(sizeof(struct date_struct));
  d->day = dd;
  d->month = mm;
  d->year = yy;
  
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

char *get_date_as_str(date d){
  char *str_date = (char *)malloc(sizeof(char)*11);
  snprintf(str_date, sizeof(char)*11, "%02d-%02d-%4d", d->day, d->month, d->year);
  return str_date;
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

date six_months_ago(date curr_date){
  date d = (date)malloc(sizeof(struct date_struct));
  d->day = curr_date->day;

  int mm = curr_date->month - 6;
  if (mm < 1){
    d->month = 12 + mm;
    d->year = curr_date->year - 1;
  }
  else{
    d->month = mm;
    d->year = curr_date->year;
  }
  return d;
}

bool date_between(date curr_date, date start_date, date end_date){

  if (start_date == NULL || end_date == NULL) /* No date interval has been set, return true*/
    return true;

  else if (compare_dates(curr_date, start_date) >= 0 && compare_dates(end_date, curr_date) >= 0)
    return true;

  else
    return false;
}