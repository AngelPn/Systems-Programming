#ifndef UTILS_H
#define UTILS_H

#include "dataStore.h"

/* Does file parsing and builds structs in dataStore */
void fileParse_and_buildStructs(char *input_dir, int bytes, dataStore *ds);

/* Sends bloom filter for each of the viruses via named pipe */
void send_bloomFilters(dataStore *ds, int write_fd, int bufferSize, int bloomSize);

/* Executes queries */
void queries(dataStore *ds, char *input_dir, int read_fd, int write_fd, int bufferSize, int bloomSize);

/* Concatenate int to string */
char *concat_int_to_str(const char str[], int i);

#endif