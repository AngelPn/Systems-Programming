#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "utils.h"
#include "HashTable.h"
#include "BloomFilter.h"
#include "ipc.h"
#include "virus_bloom.h"
#include "date.h"
#include "queries.h"

#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define RESET "\033[0m"

/*  Does proper argument handling and stores variables from command prompt to vars 
    numMonitors, socketBufferSize, cyclicBufferSize, bloomsize, input_dir, numThreads */
int argumentHandling(int argc, char **argv, int *numMonitors, int *socketBufferSize, int *cyclicBufferSize, int *bloomsize, char **input_dir, int *numThreads){

	if (argc != 13){
		printf(RED "ERROR: Invalid use of arguments, the process will terminate\n" RESET);
		return 0;			
	}
	
	if (strcmp(argv[1], "-m") == 0){
		if(argv[2] <= 0 ){
			printf(RED "ERROR: Invalid argument after -m flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*numMonitors = atoi(argv[2]);
	}
	else{
		printf(RED "ERROR: Invalid use of -m flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if (strcmp(argv[3], "-b") == 0){
		if(argv[4] <= 0 ){
			printf(RED "ERROR: Invalid argument after -b flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*socketBufferSize = atoi(argv[4]);
	}
	else{
		printf(RED "ERROR: Invalid use of -b flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if (strcmp(argv[5], "-c") == 0){
		if(argv[6] <= 0 ){
			printf(RED "ERROR: Invalid argument after -c flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*cyclicBufferSize = atoi(argv[6]);
	}
	else{
		printf(RED "ERROR: Invalid use of -c flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if(strcmp(argv[7], "-s") == 0){
		if(argv[8] <= 0 ){
			printf(RED "ERROR: Invalid argument after -s flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*bloomsize = atoi(argv[8]);
	} 			
	else{
		printf(RED "ERROR: Invalid use of -s flag, the process will terminate\n" RESET);
		return 0;			
	}

	if (strcmp(argv[9], "-i") == 0){
		*input_dir = (char *)malloc(sizeof(char)*(strlen(argv[10]) + 1));
		strcpy(*input_dir, argv[10]);
	}
	else{
		printf(RED "ERROR: Invalid use of -i flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if(strcmp(argv[11], "-t") == 0){
		if(argv[12] <= 0 ){
			printf(RED "ERROR: Invalid argument after -t flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*numThreads = atoi(argv[12]);
	} 			
	else{
		printf(RED "ERROR: Invalid use of -t flag, the process will terminate\n" RESET);
		return 0;			
	}
    return 1;
}


char *concat_int_to_string(const char str[], int i){
	char string_i[10];
	snprintf(string_i, 10, "%d", i);
	char *result = (char *)malloc(sizeof(char)*(strlen(str) + 1 + sizeof(string_i)));
	strcpy(result, str);
	strcat(result, string_i);
	return result;
}

int cmpstr(const void* p1, const void* p2){
	return strcmp(*(char* const*) p1, *(char* const*) p2);
}

void aggregation(int numMonitors, int socketBufferSize, int cyclicBufferSize, int bloomSize, char *input_dir, int numThreads){

    pid_t pid;
	pid_t monitors_pids[numMonitors]; /* stores the PIDs of the childs */
	char *names1[numMonitors], *names2[numMonitors]; /* stores the names of named pipes */
    int read_fd[numMonitors], write_fd[numMonitors]; /* stores the file descs for reading to and writing from the named pipes */

    struct dirent *subdir = NULL; /* pointer to subdirs*/

    /* Open the input dir */
    DIR *indir = NULL;
    if ((indir = opendir(input_dir)) == NULL){
        perror("Error opening input directory");
        exit(EXIT_FAILURE);
    }

	/* Get the number of subdirs in input dir */
    int numSubdirs = 0;
    while ((subdir = readdir(indir)) != NULL){
        numSubdirs++;
    }
    rewinddir(indir); /* reset the position of the directory stream */
    
	/* Store country names in countries array, removing . and .. in dirs */
	numSubdirs -= 2;
	char *countries[numSubdirs];
	int i = 0;
	while ((subdir = readdir(indir)) != NULL){
		if (strcmp(subdir->d_name, ".") == 0 || strcmp(subdir->d_name, "..") == 0)
			continue;
		else
			countries[i++] = strdup(subdir->d_name);
	}
    closedir(indir);

	/* Sort countries array alphabetically using qsort() from the C standard library */
	qsort(countries, numSubdirs, sizeof(char *), cmpstr);

    /* Create a hash table to store countries RR alphabetically per monitor */
	HashTable monitors = HTCreate(Integer, destroy_monitor);
	monitor m = NULL;
	pid_t monitor_pid;
	int monitor_idx = 0, numActiveMonitors = 0;

	for (int country_idx = 0; country_idx < numSubdirs; country_idx++){ /* For each of the countries in input_dir */

		/* Check if monitor with PID is already in hash table of monitors */
		/* If not, insert monitor (m) in hash table of monitors */
		monitor_pid = monitor_idx;
		if ((m = HTSearch(monitors, &monitor_pid, compare_monitor)) == NULL ){
			m = create_monitor(monitor_pid, monitor_idx);
			HTInsert(&(monitors), m, get_monitor_pid);
		}

		/* Assign the country subdir to monitor */
		char *countryName = countries[country_idx];
		add_country(m, countryName); /* add country in monitor to handle */
				
		if ((++monitor_idx) == numMonitors){
			monitor_idx = 0; /* reset monitor_idx */
			numActiveMonitors = 1; /* set numActiveMonitors to declare that all monitors are active */
		}
	}

	/* Update numActiveMonitors to declare the number of active monitors */
	numActiveMonitors = (numActiveMonitors == 0) ? monitor_idx : numMonitors;

	/* Create numActiveMonitors child processes with fork and sockets */
	char *arg1 = "-p", *arg2 = "-t", *arg3 = "-b", *arg4 = "-c", *arg5 = "-s";
	int port = 9000;
	char *numThreads_str = concat_int_to_string("", numThreads);
	char *socketBufferSize_str = concat_int_to_string("", socketBufferSize);
	char *cyclicBufferSize_str = concat_int_to_string("", cyclicBufferSize);
	char *bloomSize_str = concat_int_to_string("", bloomSize);

    for (int i = 0; i < numActiveMonitors; i++){
        pid = fork();

        if (pid > 0) { /* parent process */
            monitors_pids[i] = pid; /* save child's pid */

			/* Create a socket */
			if ((read_fd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				perror("Error in creating socket");
				exit(EXIT_FAILURE);
			}
        }
		else{ /* child process */
			char *port_str = concat_int_to_string("", port);

			monitor_pid = i;
			m = HTSearch(monitors, &monitor_pid, compare_monitor);

			List head = get_monitor_countries(m);

			int args_len = 12 + list_length(head);
			char *args[args_len];
			args[0] = "monitorServer";
			args[1] = arg1; args[2] = port_str;
			args[3] = arg2; args[4] = numThreads_str;
			args[5] = arg3; args[6] = socketBufferSize_str;
			args[7] = arg4; args[8] = cyclicBufferSize_str;
			args[9] = arg5; args[10] = bloomSize_str;

			int j = 11;
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

				/* Get the country name and create the path of subdir */
				char *country_name = list_node_item(head, node);
				char subdirPath[strlen(input_dir) + strlen(country_name) + 2];
				snprintf(subdirPath, sizeof(subdirPath), "%s/%s", input_dir, country_name);

				args[j] = strdup(subdirPath);
				j++;
			}
			args[j] = NULL;		

            if (execv("monitorServer", args) == -1){
				perror("Error in execv");
				exit(EXIT_FAILURE);				
			}
			free(port_str);
        }
		port++;
    }
	free(numThreads_str); free(socketBufferSize_str); 
	free(cyclicBufferSize_str); free(bloomSize_str);
    /*
     * Try to connect to the address.  For this to
     * succeed, the server must already have bound
     * this address, and must have issued a listen()
     * request.
     *
     * The third argument indicates the "length" of
     * the structure, not just the length of the
     * socket name.
     */

	struct sockaddr_in server;
	struct sockaddr* server_ptr = (struct sockaddr*)&server;

	/* Find server address */
	struct hostent* host;
	if ((host = gethostbyname("localhost")) == NULL) {
		herror("Error in gethostbyname");
		exit(EXIT_FAILURE);
	}
	
	server.sin_family = AF_INET; /* Internet domain */
	memcpy(&server.sin_addr, host->h_addr, host->h_length);

	port = 9000;
    for (int i = 0; i < numActiveMonitors; i++) {
		server.sin_port = htons(port++);
		if (connect(read_fd[i], server_ptr, sizeof(server)) < 0) {
			perror("Error in socket connect");
			exit(EXIT_FAILURE);
		}
    }

    for (int i = 0; i < numActiveMonitors; i++) {
		char *msg = receive_data(read_fd[i], socketBufferSize);
		printf("msg: %s\n", msg);
    }
	

	while (true) {}

    // /* Get bloom filters from monitors */
	// get_bloom_filters(&monitors, monitors_pids, numActiveMonitors, socketBufferSize, bloomSize, read_fd);

	// /* Run queries */
	// run_queries(&monitors, socketBufferSize, bloomSize, monitors_pids, read_fd, write_fd, numActiveMonitors, input_dir);

    // /* Kill monitors, delete named pipes and remove tmp dir */
    // for (int i = 0; i < numMonitors; i++){
	// 	kill(monitors_pids[i], SIGKILL);
    //     unlink(names1[i]); unlink(names2[i]);
    //     free(names1[i]); free(names2[i]);
    // }
	// rmdir("./tmp");
	// free(input_dir);
	
    // /* Wait until all the children are dead */
    // for (int i = 0; i < numMonitors; i++){
    //     wait(&monitors_pids[i]);
    // }

	// /* Create the LogFiles dir with read/write/search permissions for owner, group and others */
    // mkdir("./LogFiles", S_IRWXU | S_IRWXG | S_IRWXO);

	// int accepted = 0, rejected = 0;
	// m = NULL;
	// List head = NULL, m_countries = NULL;

	// /* Traverse Hash Table of monitors */
	// for (int i = 0; i < HTSize(monitors); i++){
	// 	if ((head = get_HTchain(monitors, i)) != NULL){
	// 		for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

	// 			/* Create the log_file.xxx for monitor with PID xxx and open it */
	// 			m = list_node_item(head, node);
	// 			char *filepath = concat_int_to_string("./LogFiles/log_file.", *((int *)get_monitor_pid(m)));
	// 			FILE *logfile = NULL;
	// 			if ((logfile = fopen(filepath, "w")) == NULL){
	// 				perror(RED "Error opening Log file"  RESET);
	// 				exit(EXIT_FAILURE);
	// 			}

	// 			/* Write to log_file.xxx the countries that monitor handles */
	// 			m_countries = get_monitor_countries(m);
	// 			for (ListNode n = list_first(m_countries); n != NULL; n = list_next(m_countries, n)){
	// 				fprintf(logfile, "%s\n", (char *)list_node_item(m_countries, n));
	// 			}

	// 			/* Write to log_file.xxx the total number of requests */
	// 			accepted = get_total_accepted(m);
	// 			rejected = get_total_rejected(m);
	// 			fprintf(logfile, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n", accepted+rejected, accepted, rejected);
				
	// 			fclose(logfile);
	// 			free(filepath);
	// 			accepted = rejected = 0;
	// 		}
	// 	}
	// }

	/* Deallocate memory */
	HTDestroy(monitors);	
}

/* Gets bloom filters from specified file descriptor (fd_index) */
void read_bloom_filters(int fd_index, monitor m, int bufferSize, int bloomSize, int *read_fd){

	virus_bloom v = NULL;
	char *virus_name = NULL, *bloom_filter = NULL;

	while (true){
		virus_name = receive_data(read_fd[fd_index], bufferSize);
		if (!strcmp(virus_name, "ready")){
			free(virus_name);
			break;
		}

		/* Check if virus is already in hash table of viruses of monitor */
		/* If not, insert virus_bloom (v) in hash table of viruses of monitor */
		if ((v = HTSearch(get_monitor_viruses(m), virus_name, compare_virus_bloomName)) == NULL){
			v = create_virus_bloom(virus_name, bloomSize);
			add_virus(m, v);
		}

		bloom_filter = receive_BloomFilter(read_fd[fd_index], bufferSize);
		update_BloomFilter(v, bloom_filter);

		free(bloom_filter);
		free(virus_name);
	}
}

void get_bloom_filters(HashTable *monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd){

    fd_set active, ready; /* represents file descriptor sets for the select function */
    FD_ZERO(&active); /* initializes the file descriptor set 'active' to be the empty set */

	/* Add file descriptors in read_fd array to the file descriptor set 'active' */
    for (int i = 0; i < numActiveMonitors; i++){
		FD_SET(read_fd[i], &active);
	}
	
	monitor m = NULL;
    int counter = 0;

	/* For each of the active monitors, get the incoming bloom filters */
    while (counter < numActiveMonitors){

        /* Find out how many monitors are ready */
		ready = active;
        if (select(FD_SETSIZE, &(ready), NULL, NULL, NULL) < 0){
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        /* For all the monitors that have already sent bloom filters */
        for (int i = 0; i < numActiveMonitors; i++){

			if (!(FD_ISSET(read_fd[i], &ready)))
				continue;

			/* Get the monitor with specified PID */
			m = HTSearch(*monitors, &(monitors_pids[i]), compare_monitor);

			/* Read the bloom filters from the pipe */
			read_bloom_filters(i, m, bufferSize, bloomSize, read_fd);

            counter++;
			FD_CLR(read_fd[i], &active);
        }
    }
}


void reborn_child(HashTable *monitors, pid_t *monitors_pids, int bufferSize, int bloomSize, int *read_fd, int *write_fd, int numActiveMonitors, char *input_dir){

	pid_t dead, reborn;
	monitor m = NULL, reborn_m = NULL;
	char *name1, *name2;
	int i = 0;
	
	/* Loop all the children, to see if they have died */
	while ((dead = waitpid(-1, NULL, WNOHANG)) > 0) {

		/* Get the position (fd index) of dead child */
		for (i = 0; i < numActiveMonitors; i++){
			if (dead == monitors_pids[i])
				break;
		}

		reborn = fork();

        /* Store the name of the 2 named pipes */
		name1 = concat_int_to_string("./tmp/myfifo1_", i);
		name2 = concat_int_to_string("./tmp/myfifo2_", i);

        if (reborn > 0) { /* parent process */
            monitors_pids[i] = reborn; /* save child's pid */
        }
		else{ /* child process */
            if (execl("Monitor", "Monitor", name1, name2, NULL) == -1){
				perror("Error in execl");
				exit(EXIT_FAILURE);				
			}
        }

		/* Child has gone to another executable, close the old file descriptors */
		close(read_fd[i]);
		close(write_fd[i]);

		/* Οpen the named pipes, store the file descs and send init data */
		if ((read_fd[i] = open(name1, O_RDONLY, 0666)) == -1) {
			perror("Error storing file desc in reading array");
			exit(EXIT_FAILURE);
		}
		if ((write_fd[i] = open(name2, O_WRONLY, 0666)) == -1) {
			perror("Error storing file desc in writing array");
			exit(EXIT_FAILURE);
		}
		free(name1); free(name2);	
		send_init(write_fd[i], bufferSize, bloomSize, input_dir);

		/* Get dead monitor from Hash Table and reborn it */
		m = HTSearch(*monitors, &dead, compare_monitor);
		reborn_m = create_copy_monitor(reborn, m);
		HTInsert(monitors, reborn_m, get_monitor_pid);
		HTDelete(*monitors, get_monitor_pid(m), compare_monitor, destroy_copy_monitor);
		
		/* Send the names of countries that new child will handle through pipe */
		List m_countries = get_monitor_countries(reborn_m);
		for (ListNode node = list_first(m_countries); node != NULL; node = list_next(m_countries, node)){
			send_data(write_fd[i], bufferSize, (char *)list_node_item(m_countries, node), 0);
		}
		send_data(write_fd[i], bufferSize, "end", 0);

		/* Read the bloom filters the reborn child has sent */
		read_bloom_filters(i, reborn_m, bufferSize, bloomSize, read_fd);		
	}
}
