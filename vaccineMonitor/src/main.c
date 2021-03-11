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

    char *buf = "erhhertwhrt";
    char *token = strtok(buf, "-");
    int tok = atoi(token);
    char *tok2 = strtok(NULL, "-");
    printf("%s %d %s\n", token, tok, tok2);

    // dataStore ds;
    // create_structs(&ds);

    // fileParse_and_buildStructs(filepath, bloom_size, &ds);

    // queries(bloom_size, &ds);

    // print_ht_viruses(&ds);

    // /* Deallocate memory */
    // destroy_structs(&ds);

	return 0;
}