#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include "dataStore.h"
#include "ipc.h"
#include "country.h"
#include "utils_queries.h"
#include "CyclicBuffer.h"

/* Global variables for our buffer mutexes and condition variables */
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t nonempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t nonfull = PTHREAD_COND_INITIALIZER;

/* Shared variable that keeps the structures needed for queries from client */
dataStore ds;

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
    
	/* Create a cyclic buffer to store the paths */
	CyclicBuffer buffer = BuffCreate(cyclicBufferSize, paths, paths_len);

	/* Create the structs needed for queries */
	create_structs(&ds, bloomSize);

	/* Allocate space to store the thread ids */
	pthread_t *thread_ids = malloc(sizeof(pthread_t)*numThreads);
	if (thread_ids == NULL){
		perror("Error in allocating thread_ids");
		exit(EXIT_FAILURE);
	}

	// first things first: create n threads in order to serve simulatneously
	int res;
	for (int i = 0; i < numThreads; i++) {
		if ((res = pthread_create(&thread_ids[i], NULL, fileParse_and_buildStructs, buffer))) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}
    

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


    /* Deallocate memory */
    // destroy_structs(&ds);
	BuffDestroy(buffer);

	return 0;
}