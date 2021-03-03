#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils.h"
#include "../include/virus.h"
#include "../include/citizenRecord.h"

int main(int argc, char **argv){

    int bloom_size; char *filepath = NULL;
    if(!argumentHandling(argc, argv, &bloom_size, &filepath))
        exit(EXIT_FAILURE);

    //printf("%s, %d\n", filepath, bloom_size);	

    HashTable citizens = HTCreate(Integer, destroy_citizen);
    HashTable viruses = HTCreate(String, destroy_virus);

    fileParse_and_buildStructs(filepath, &citizens, &viruses);

    printf("Print hash table of citizens:\n");
    HTPrint(citizens, print_citizen);

    printf("\nPrint hash table of viruses:\n");
    HTPrint(viruses, print_virus);

    printf("Destroy hash table of citizens\n");
    HTDestroy(citizens);

    printf("Destroy hash table of viruses\n");
    HTDestroy(viruses);

	return 0;
}