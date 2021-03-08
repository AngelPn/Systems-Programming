#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "dataStore.h"

int main(int argc, char **argv){

    int bloom_size; char *filepath = NULL;
    if(!argumentHandling(argc, argv, &bloom_size, &filepath))
        exit(EXIT_FAILURE);

    //printf("%s, %d\n", filepath, bloom_size);	

    dataStore ds;
    create_structs(&ds);

    fileParse_and_buildStructs(filepath, bloom_size, &ds);

    queries(bloom_size, &ds);

    // printf("Print hash table of citizens:\n");
    // HTPrint(citizens, print_citizen);

    // printf("\nPrint hash table of viruses:\n");
    // HTPrint(viruses, print_virus);

    /* Deallocate memory */
    destroy_structs(&ds);

	return 0;
}