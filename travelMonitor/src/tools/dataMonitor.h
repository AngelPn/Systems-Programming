#ifndef DATAMONITOR_H
#define DATAMONITOR_H

#include <sys/types.h>

#include "HashTable.h"
#include "List.h"

typedef struct dataMonitor_struct
{
    pid_t pid;
    List countries;
    HashTable viruses;

} dataMonitor;

/* Creates dataMonitor_struct */
dataMonitor create_dataMonitor(pid_t pid);

/* Returns pointer to pid of dataMonitor_struct */
void *get_dataMonitor_pid(void *m);

/* Returns the list of countries of dataMonitor */
List get_dataMonitor_countries(void *m);

/* Returns the hash table of viruses of dataMonitor */
HashTable get_dataMonitor_viruses(void *m);

/* Adds country in list of countries in dataMonitor */
void add_dataMonitor_country(dataMonitor m, char *country);

/*Prints the fields of dataMonitor */
void print_dataMonitor(void *m);

/* Compares key with pied of dataMonitor v */
int compare_dataMonitor(void *key, void *m);

/* Deallocates memory of dataMonitor_struct */
void destroy_dataMonitor(void *m);

#endif