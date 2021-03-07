#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "virus.h"
#include "country.h"
#include "citizenRecord.h"

int main(int argc, char **argv){

    int bloom_size; char *filepath = NULL;
    if(!argumentHandling(argc, argv, &bloom_size, &filepath))
        exit(EXIT_FAILURE);

    //printf("%s, %d\n", filepath, bloom_size);	

    HashTable citizens = HTCreate(Integer, destroy_citizen);
    HashTable viruses = HTCreate(String, destroy_virus);
    HashTable countries = HTCreate(String, destroy_country);

    fileParse_and_buildStructs(filepath, bloom_size, &citizens, &viruses, &countries);

    queries(bloom_size, &citizens, &viruses, &countries);

    printf("Print hash table of citizens:\n");
    HTPrint(citizens, print_citizen);

    printf("\nPrint hash table of viruses:\n");
    HTPrint(viruses, print_virus);

    /* Deallocate memory */
    HTDestroy(countries);
    HTDestroy(citizens);
    HTDestroy(viruses); 

	return 0;
}