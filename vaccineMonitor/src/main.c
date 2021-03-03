#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils.h"

int main(int argc, char **argv){

    int bloom_size; char *filepath = NULL;
    if(!argumentHandling(argc, argv, &bloom_size, &filepath))
        exit(EXIT_FAILURE);

    printf("%s, %d\n", filepath, bloom_size);	

    HashTable citizens = HTCreate(Integer, destroy_citizen);
    HashTable viruses = HTCreate(String, destroy_virus);

    fileParse_and_buildStructs(filepath, &citizens, &viruses);

    // printf("Print Hash table\n");
    // HTPrint(HTcitizens, print_record);

    printf("Destroy Hash table\n");
    HTDestroy(citizens);

	return 0;
}