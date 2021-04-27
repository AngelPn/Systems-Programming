#ifndef UTILS_H
#define UTILS_H

#include "dataStore.h"

/* Does file parsing and builds structs in dataStore */
void fileParse_and_buildStructs(char *input_dir, int bytes, dataStore *ds);

/* Sends bloom filter for each of the viruses via named pipe */
void send_bloomFilters(dataStore *ds);

/* Executes queries */
void queries(int bytes, dataStore *ds);

#endif