#ifndef UTILS_H
#define UTILS_H

/*  Does proper argument handling and stores variables from command prompt to vars 
    numMonitors, buffersize, bloomsize, input_dir */
int argumentHandling(int argc, char **argv, int *numMonitors, int *buffersize, int *bloomsize, char **input_dir);

void aggregator(int numMonitors, int bufferSize, int bloomSize, char *input_dir);

void reborn_child(HashTable *monitors, pid_t *monitors_pids, int *read_fd, int *write_fd, int numActiveMonitors);

#endif