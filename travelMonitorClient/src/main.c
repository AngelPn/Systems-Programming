#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char **argv){
    
	int numMonitors, socketBufferSize, cyclicBufferSize, bloomSize, numThreads;
    char *input_dir = NULL;
    if(!argumentHandling(argc, argv, &numMonitors, &socketBufferSize, &cyclicBufferSize, &bloomSize, &input_dir, &numThreads))
        exit(EXIT_FAILURE);

    // printf("numMonitors: %d, numThreads: %d, socketBufferSize: %d, cyclicBufferSize: %d, bloomSize: %d, input_dir: %s\n", numMonitors, numThreads, socketBufferSize, cyclicBufferSize, bloomSize, input_dir);
    aggregation(numMonitors, socketBufferSize, cyclicBufferSize, bloomSize, input_dir, numThreads);

	return 0;
}