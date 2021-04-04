#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "dataStore.h"

int main(int argc, char **argv){

    int bloom_size; char *filepath = NULL;
    if(!argumentHandling(argc, argv, &bloom_size, &filepath))
        exit(EXIT_FAILURE);

    dataStore ds;
    create_structs(&ds);

    fileParse_and_buildStructs(filepath, bloom_size, &ds);

    queries(bloom_size, &ds);

    /* Deallocate memory */
    destroy_structs(&ds);

	return 0;
}