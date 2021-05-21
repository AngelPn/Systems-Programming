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
    // fprintf(stderr, "child %d\n", getpid());

    /* Get arguments */
    int port, numThreads, socketBufferSize, cyclicBufferSize, bloomSize, paths_len;
	char **paths;
    argsHandling(argc, argv, &port, &numThreads, &socketBufferSize, &cyclicBufferSize, &bloomSize, &paths, &paths_len);
        

	// printf("port: %d, numThreads: %d, socketBufferSize: %d, cyclicBufferSize: %d, bloomSize: %d\n", port, numThreads, socketBufferSize, cyclicBufferSize, bloomSize);
	// for (int i = 0; i < paths_len; i++){
	// 	printf("%s\n", paths[i]);
	// }
    
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
		// printf("here\n");
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
	// print_ht_citizens(&ds);
	/* Initialize our service */
	struct sockaddr_in server, client, ip;
	socklen_t server_len = sizeof(struct sockaddr_in);
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

	struct hostent* host;
	if ((host = gethostbyname("localhost")) == NULL) {
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

	// printf("I am PID %d waiting for request at port %d\n",(int)getpid(), port);

	/* Accept the connection */
	int conn_fd;
	if ((conn_fd = accept(sock_fd, clientptr, &client_len)) < 0) {
		perror("Error in accept");
		exit(EXIT_FAILURE);
	}
	/* The connection comes from the parent process and we want to learn his ip */
	socklen_t len = sizeof(ip);
	getpeername(conn_fd, (struct sockaddr *) &ip, &len);

	printf("Accepted connection\n");

	// send_data(conn_fd, socketBufferSize, "Hello World", 0);

	/* Send bloom filters to parent process */
	printf("ABOUT TO SEND BLOOM FILTERS\n");
	send_bloomFilters(conn_fd, socketBufferSize, bloomSize);
	printf("BLOOM FILTERS SENT\n");
	while (true) { }
	// /* Execute queries*/
	// queries(&ds, input_dir, read_fd, write_fd, socketBufferSize, bloomSize);

	// print_ht_citizens(&ds);
	close(conn_fd);
	close(sock_fd);
	

    /* Deallocate memory */
    destroy_structs(&ds);
	BuffDestroy(buffer);

	return 0;
}