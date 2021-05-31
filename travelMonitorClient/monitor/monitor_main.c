#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "dataStore.h"
#include "ipc.h"
#include "country.h"
#include "utils_queries.h"
#include "CyclicBuffer.h"
#include "List.h"


/* Global variables for our buffer mutexes and condition variables */
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t nonempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t nonfull = PTHREAD_COND_INITIALIZER;

/* Shared variables */
dataStore ds; 	/* keeps the structures needed for queries from client */
int quit = 0; 	/* signals thread termination */


int main(int argc, char **argv){

    /* Get arguments */
    int port, numThreads, socketBufferSize, cyclicBufferSize, bloomSize, subdirPaths_len;
	char **subdirPaths;
    argsHandling(argc, argv, &port, &numThreads, &socketBufferSize, &cyclicBufferSize, &bloomSize, &subdirPaths, &subdirPaths_len);

	/* Create the structs needed for queries */
	create_structs(&ds, bloomSize);	

	List filePaths = get_filepaths(subdirPaths, subdirPaths_len);
    
	/* Create a cyclic buffer to store the paths */
	CyclicBuffer buffer = BuffCreate(cyclicBufferSize);

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

	/* Store the expected number of files to be read */
	int total_filepaths = list_length(filePaths);

	/* While filepath exists that it has not been read */
	while(list_length(filePaths)){

		pthread_mutex_lock(&mtx); /* shared data area */

		/* If buffer is full, wait for signal nonfull */
		while (BuffFull(buffer)) {
			pthread_cond_wait(&nonfull, &mtx);
		}

		ListNode node = list_first(filePaths);
		BuffInsert(buffer, list_node_item(filePaths, node));
		list_remove(filePaths, node);

		pthread_mutex_unlock(&mtx);

		/* The buffer is not empty anymore (if it was) */
		pthread_cond_signal(&nonempty);
	}

	/* Wait for all the filepaths to get parsed */
	while (!BuffTotal(buffer, total_filepaths)) { }

	/* Initialize our service */
	struct sockaddr_in server, client, ip;
	socklen_t client_len = sizeof(struct sockaddr_in);
	struct sockaddr* serverptr = (struct sockaddr*) &server;
	struct sockaddr* clientptr = (struct sockaddr*) &client;

	/* Create socket connection */
	int sock_fd;
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error in creating socket");
		exit(EXIT_FAILURE);
	}

	/* Enable re-binding this address */
	int set = 1;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));

	/* Assign IP, port */
	server.sin_family = AF_INET; /* Internet domain */

	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);
	struct hostent* host;
	if ((host = gethostbyname(hostname)) == NULL) {
		herror("Error in gethostbyname");
		exit(EXIT_FAILURE);
	}

	memcpy(&server.sin_addr, host->h_addr, host->h_length);
	server.sin_port = htons(port);

	/* Bind socket to address */
	if (bind(sock_fd, serverptr, sizeof(server)) < 0) {
		perror("Error in bind");
		exit(EXIT_FAILURE);
	}
	
	/* Server listening... */
	if (listen(sock_fd, 5) < 0) {
		perror("Error in listen");
		exit(EXIT_FAILURE);
	}

	/* Accept the connection */
	int conn_fd;
	if ((conn_fd = accept(sock_fd, clientptr, &client_len)) < 0) {
		perror("Error in accept");
		exit(EXIT_FAILURE);
	}
	/* The connection comes from the parent process and we want to learn his ip */
	socklen_t len = sizeof(ip);
	getpeername(conn_fd, (struct sockaddr *) &ip, &len);

	/* Send bloom filters to parent process */
	send_bloomFilters(conn_fd, socketBufferSize, bloomSize);

	/* Execute queries*/
	queries(conn_fd, buffer, socketBufferSize, bloomSize, total_filepaths);

	/* Unblock threads and signal their termination */
	pthread_mutex_lock(&mtx);
	quit = 1;
	pthread_cond_broadcast(&nonempty);
	pthread_mutex_unlock(&mtx);
	
	for (int i = 0; i < numThreads; i++) {
		pthread_join(thread_ids[i], NULL);
	} 

	close(conn_fd);
	close(sock_fd);
	pthread_cond_destroy(&nonempty); pthread_cond_destroy(&nonfull);
	pthread_mutex_destroy(&mtx);	

    /* Deallocate memory */
	free(subdirPaths);
	list_destroy(filePaths);
    destroy_structs(&ds);
	BuffDestroy(buffer);

	return 0;
}