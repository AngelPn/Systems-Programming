#ifndef MONITOR_H
#define MONITOR_H

#include <sys/types.h>

#include "List.h"

/* "struct monitor_struct" is incomplete struct */
typedef struct monitor_struct *monitor;

/* Creates monitor_struct */
monitor create_monitor(pid_t pid);

/* Returns pointer to pid of monitor_struct */
void *get_monitor_pid(void *m);

/* Returns list of countries of monitor */
List get_monitor_countries(void *m);

/* Adds country in list of countries in monitor */
void add_country(monitor m, char *country);

/*Prints the fields of monitor */
void print_monitor(void *m);

/* Compares key with pied of monitor v */
int compare_monitor(void *key, void *m);

/* Deallocates memory of monitor_struct */
void destroy_monitor(void *m);

#endif