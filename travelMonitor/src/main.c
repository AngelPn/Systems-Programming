#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char **argv){
    
	int numMonitors, bufferSize, bloomSize;
    char *input_dir = NULL;
    if(!argumentHandling(argc, argv, &numMonitors, &bufferSize, &bloomSize, &input_dir))
        exit(EXIT_FAILURE);

    aggregation(numMonitors, bufferSize, bloomSize, input_dir);

	return 0;
}