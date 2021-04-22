#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char **argv){
    
	int n_monitors, buffer_size, bloom_size;
    char *input_dir = NULL;
    if(!argumentHandling(argc, argv, &n_monitors, &buffer_size, &bloom_size, &input_dir))
        exit(EXIT_FAILURE);

    printf("%d %d %d %s\n", n_monitors, buffer_size, bloom_size, input_dir);

	return 0;
}