#ifndef _ARGHAND_H_
#define _ARGHAND_h_

#include "HashTable.h"

int argumentHandling(int argc, char **argv, int *bloomsize, char **filepath);

void fileParse_and_buildStructs(char *filepath, int kilobytes, HashTable *citizens, HashTable *viruses, HashTable *countries);

void queries(int kilobytes, HashTable *citizens, HashTable *viruses, HashTable *countries);

#endif