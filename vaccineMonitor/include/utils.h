#ifndef _ARGHAND_H_
#define _ARGHAND_h_

#include "HashTable.h"

int argumentHandling(int ,char **, int* , char **);

int fileParse_and_buildStructs(char *filepath, HashTable *citizens, HashTable *viruses);

#endif