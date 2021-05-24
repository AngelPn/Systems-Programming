#ifndef UTILS_H
#define UTILS_H

#include "dataStore.h"
#include "List.h"

/*  Does proper argument handling and stores variables from command prompt to vars 
    numThreads, socketBufferSize, cyclicBufferSize, bloomsize, paths */
void argsHandling(int argc, char **argv, int *port, int *numThreads, int *socketBufferSize, int *cyclicBufferSize, int *bloomsize, char ***paths, int *paths_len);

/* Returns a list of filepaths given the subdir paths */
List get_filepaths(char **subdirPaths, int subdirPaths_len);

/* Does file parsing and builds structs in dataStore */
void *fileParse_and_buildStructs(void *buff);

/* Sends bloom filter for each of the viruses via named pipe */
void send_bloomFilters(int write_fd, int bufferSize, int bloomSize);

/* Executes queries */
void queries(int conn_fd, CyclicBuffer buffer, int bufferSize, int bloomSize);

/* Concatenate int to string */
char *concat_int_to_str(const char str[], int i);

#endif