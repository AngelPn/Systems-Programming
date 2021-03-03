#ifndef _ARGHAND_H_
#define _ARGHAND_h_

#include "HashTable.h"
#include "common_types.h"
#include "SkipList.h"
#include "virus.h"
#include "citizenRecord.h"

int argumentHandling(int argc, char **argv, int *bloomsize, char **filepath);

void fileParse_and_buildStructs(char *filepath, HashTable *citizens, HashTable *viruses);

#endif