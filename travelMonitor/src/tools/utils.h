#ifndef UTILS_H
#define UTILS_H

#include "HashTable.h"
#include "monitor.h"

/*  Does proper argument handling and stores variables from command prompt to vars 
    numMonitors, buffersize, bloomsize, input_dir */
int argumentHandling(int argc, char **argv, int *numMonitors, int *buffersize, int *bloomsize, char **input_dir);

/* Creates monitors, named pipes, executes queries and exits */
void aggregation(int numMonitors, int bufferSize, int bloomSize, char *input_dir);

/* Gets bloom filters from named pipe with specified fd */
void read_bloom_filters(int fd_index, monitor m, int bufferSize, int bloomSize, int *read_fd);

/* Gets sent bloom filters from monitors to parent process */
void get_bloom_filters(HashTable *monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd);

/* Reborns a dead child process */
void reborn_child(HashTable *monitors, pid_t *monitors_pids, int bufferSize, int bloomSize, int *read_fd, int *write_fd, int numActiveMonitors, char *input_dir);

#endif