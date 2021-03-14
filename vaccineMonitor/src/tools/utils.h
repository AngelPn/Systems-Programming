#ifndef ARGHAND_H
#define ARGHAND_H

#include "dataStore.h"

/* Does proper argument handling and stores variables from command prompt to vars bloomsize, filepath */
int argumentHandling(int argc, char **argv, int *bloomsize, char **filepath);

/* Does file parsing and builds structs in dataStore */
void fileParse_and_buildStructs(char *filepath, int bytes, dataStore *ds);

/* Executes queries */
void queries(int bytes, dataStore *ds);

#endif