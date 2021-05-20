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

	/* Create threads */
	int res;
	for (int i = 0; i < numThreads; i++) {
		if ((res = pthread_create(&thread_ids[i], NULL, fileParse_and_buildStructs, buffer))) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	/* While there is an empty space in buffer
	   and at least one path exists that it has not been read */
	while(empty_space_in_buff(buffer)){
		pthread_mutex_lock(&mtx); /* shared data area */

		/* If buffer is full, wait for signal nonfull */
		while (BuffFull(buffer)) {
			pthread_cond_wait(&nonfull, &mtx);
		}

		BuffAdd(buffer);

		pthread_mutex_unlock(&mtx);

		/* The buffer is not empty anymore (if it was) */
		pthread_cond_signal(&nonempty);		
	}

	// /* Send bloom filters to parent process */
	// send_bloomFilters(&ds, write_fd, socketBufferSize, bloomSize);

	// /* Execute queries*/
	// queries(&ds, input_dir, read_fd, write_fd, socketBufferSize, bloomSize);


    /* Deallocate memory */
    destroy_structs(&ds);
	BuffDestroy(buffer);

	return 0;
}