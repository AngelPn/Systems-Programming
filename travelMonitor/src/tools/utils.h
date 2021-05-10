#ifndef UTILS_H
#define UTILS_H

#include "HashTable.h"

/*  Does proper argument handling and stores variables from command prompt to vars 
    numMonitors, buffersize, bloomsize, input_dir */
int argumentHandling(int argc, char **argv, int *numMonitors, int *buffersize, int *bloomsize, char **input_dir);

/* Creates monitors, named pipes and executes queries */
void aggregator(int numMonitors, int bufferSize, int bloomSize, char *input_dir);

/* Gets sent bloom filters from monitors to parent process */
void get_bloom_filters(HashTable *monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd);

/* Reborns a dead child process */
void reborn_child(HashTable *monitors, pid_t *monitors_pids, int bufferSize, int bloomSize, int *read_fd, int *write_fd, int numActiveMonitors, char *input_dir);

#endif