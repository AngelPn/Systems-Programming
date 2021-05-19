#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "dataStore.h"
#include "ipc.h"
#include "country.h"
#include "utils_queries.h"

int main(int argc, char **argv){
    fprintf(stderr, "child %d\n", getpid());

    /* Get arguments */
    int port, numThreads, socketBufferSize, cyclicBufferSize, bloomSize, paths_len;
	char **paths;
    argsHandling(argc, argv, &port, &numThreads, &socketBufferSize, &cyclicBufferSize, &bloomSize, &paths, &paths_len);
        

	printf("port: %d, numThreads: %d, socketBufferSize: %d, cyclicBufferSize: %d, bloomSize: %d\n", port, numThreads, socketBufferSize, cyclicBufferSize, bloomSize);
	for (int i = 0; i < paths_len; i++){
		printf("%s\n", paths[i]);
	}
    
    // /* Structures needed for queries */
    // dataStore ds;
    // create_structs(&ds);

	// /* Read the dirs from the pipe */
    // country c;
	// char *country_name;	
	// while (true) {
	// 	country_name = receive_data(read_fd, socketBufferSize);
	// 	if (!strcmp(country_name, "end")){
	// 		free(country_name);
	// 		break;
	// 	}

	// 	/* Check if country is already in hash table of countries */
	// 	/* If not, insert country (c) in hash table of countries */
	// 	if ((c = HTSearch(ds.countries, country_name, compare_countries)) == NULL ){
	// 		c = create_country(country_name);
	// 		HTInsert(&(ds.countries), c, get_country_name);
	// 	}
	// 	free(country_name);
	// }


	// /* Parse the files and build the structs */
	// fileParse_and_buildStructs(input_dir, bloomSize, &ds);

	// /* Send bloom filters to parent process */
	// send_bloomFilters(&ds, write_fd, socketBufferSize, bloomSize);

	// /* Execute queries*/
	// queries(&ds, input_dir, read_fd, write_fd, socketBufferSize, bloomSize);


    // /* Deallocate memory */
    // destroy_structs(&ds);

	return 0;
}