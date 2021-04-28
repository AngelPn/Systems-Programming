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
#include <string.h>
#include <signal.h>
#include <sys/select.h>

#include "utils.h"
#include "HashTable.h"
#include "monitor.h"
#include "ipc.h"
#include "virus_bloom.h"

#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define RESET "\033[0m"

/*  Does proper argument handling and stores variables from command prompt to vars 
    numMonitors, buffersize, bloomsize, input_dir */
int argumentHandling(int argc, char **argv, int *numMonitors, int *buffersize, int *bloomsize, char **input_dir){

	if (argc != 9){
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
			*buffersize = atoi(argv[4]);
	}
	else{
		printf(RED "ERROR: Invalid use of -b flag, the process will terminate\n" RESET);				
		return 0;				
	}

	if(strcmp(argv[5], "-s") == 0){
		if(argv[6] <= 0 ){
			printf(RED "ERROR: Invalid argument after -s flag. Number must be positive integer.\n" RESET);
			return 0;
		}
		else
			*bloomsize = atoi(argv[6]);
	} 			
	else{
		printf(RED "ERROR: Invalid use of -s flag, the process will terminate\n" RESET);
		return 0;			
	}

	if (strcmp(argv[7], "-i") == 0){
		*input_dir = (char *)malloc(sizeof(char)*(strlen(argv[8]) + 1));
		strcpy(*input_dir, argv[8]);
	}
	else{
		printf(RED "ERROR: Invalid use of -i flag, the process will terminate\n" RESET);				
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

void get_bloom_filters(HashTable monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd);

void aggregator(int numMonitors, int bufferSize, int bloomSize, char *input_dir){

    pid_t pid;
	pid_t monitors_pids[numMonitors]; /* stores the PIDs of the childs */
    int read_fd[numMonitors], write_fd[numMonitors]; /* stores the file descs for reading and writing */
	char *names1[numMonitors], *names2[numMonitors]; /* stores the names of named pipes */

	/* Create a temporary dir with read/write/search permissions for owner, group and others */
    mkdir("./tmp", S_IRWXU | S_IRWXG | S_IRWXO);

	/* Create numMonitors child processes with fork and named pipes */
    for (int i = 0; i < numMonitors; i++) {
        pid = fork();

        /* Store the name of the 2 named pipes */
		names1[i] = concat_int_to_string("./tmp/myfifo1_", i);
		names2[i] = concat_int_to_string("./tmp/myfifo2_", i);

        if (pid > 0) { /* parent process */
            monitors_pids[i] = pid; /* save child's pid */

            /* Create named pipes */
            if ((mkfifo(names1[i], 0666) == -1) && (errno != EEXIST)) {
				perror("Error creating named pipe");
				exit(EXIT_FAILURE);
            }
            if ((mkfifo(names2[i], 0666) == -1) && (errno != EEXIST)) {
				perror("Error creating named pipe");
				exit(EXIT_FAILURE);
            }
        }
		else{ /* child process */
			/* 	Replace the current running process with a new process
				representing the argument list available to the executed program */
            if (execl("Monitor", "Monitor", names1[i], names2[i], NULL) == -1){
				perror("Error in execl");
				exit(EXIT_FAILURE);				
			}
        }
    }

    /* Οpen the named pipes, store the file descs and send init data */
    for (int i = 0; i < numMonitors; i++) {
		char *name1 = concat_int_to_string("./tmp/myfifo1_", i);
		char *name2 = concat_int_to_string("./tmp/myfifo2_", i);
        if ((read_fd[i] = open(name1, O_RDONLY, 0666)) == -1) {
            perror("Error storing file desc in reading array");
            exit(EXIT_FAILURE);
        }
        if ((write_fd[i] = open(name2, O_WRONLY, 0666)) == -1) {
            perror("Error storing file desc in writing array");
            exit(EXIT_FAILURE);
        }
		send_init(write_fd[i], bufferSize, bloomSize, input_dir);
		printf("monitors_pids[%d] = %d\n", i, monitors_pids[i]);
        free(name1);
        free(name2);
    }
	printf("(utils)Opened named pipes and stored the file descs\n");
    struct dirent *subdir; /* pointer to subdirs*/

    /* Open the input dir */
    DIR *indir;
    if ((indir = opendir(input_dir)) == NULL) {
        perror("Error opening input directory");
        exit(EXIT_FAILURE);
    }

	/* Get the number of subdirs in input dir */
    int numSubdirs = 0;
    while ((subdir = readdir(indir)) != NULL) {
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
    closedir(indir); free(input_dir);

	/* Sort countries array alphabetically using qsort() from the C standard library */
	qsort(countries, numSubdirs, sizeof(char *), cmpstr);

	// for (int i=0; i < numSubdirs; i++)
	// 	printf("%d. %s\n", i, countries[i]);

    /* Create a hash table to store countries RR alphabetically per monitor */
	HashTable monitors = HTCreate(Integer, destroy_monitor);
	int monitor_idx = 0, numActiveMonitors = 0;
	for (int country_idx = 0; country_idx < numSubdirs; country_idx++){
		monitor m = NULL;
		pid_t monitor_pid = monitors_pids[monitor_idx];

		/* Check if monitor with PID is already in hash table of monitors */
		/* If not, insert monitor (m) in hash table of monitors */
		if ((m = HTSearch(monitors, &monitor_pid, compare_monitor)) == NULL ){
			m = create_monitor(monitor_pid);
			HTInsert(&(monitors), m, get_monitor_pid);
		}
		/* Assign the country subdir to monitor */
		add_country(m, countries[country_idx]); /* add country in monitor to handle */
        send_data(write_fd[monitor_idx], bufferSize, countries[country_idx]); /* inform the child process through the pipe */
		
		if ((++monitor_idx) == numMonitors){
			monitor_idx = 0; /* reset monitor_idx */
			numActiveMonitors = 1; /* set numActiveMonitors to declare that all monitors are active */
		}
			
	}

    /* Write 'end' into every pipe to note the end of distribution of countries per monitor */
    for (int i = 0; i < numMonitors; i++){
		send_data(write_fd[i], bufferSize, "end");
	}

	/* Update numActiveMonitors to declare the number of active monitors */
	numActiveMonitors = (numActiveMonitors == 0) ? monitor_idx : numMonitors;
	// printf("numActiveMonitors: %d\n", numActiveMonitors);

    /* Get bloom filters from monitors */
	get_bloom_filters(monitors, monitors_pids, numActiveMonitors, bufferSize, bloomSize, read_fd);

	// while(true){
	// 	// printf("sad");
	// }

	printf("Print monitors hash table in parent\n");
	HTPrint(monitors, print_monitor);
	HTDestroy(monitors);

    /* Send a SIGKILL to the monitors to end them */
    for (int i = 0; i < numMonitors; i++) {
        kill(monitors_pids[i], SIGKILL);
    }
    /* Delete named pipes */
    for (int i = 0; i < numMonitors; i++) {
        unlink(names1[i]);
        unlink(names2[i]);
     
        free(names1[i]);
        free(names2[i]);
    }
    /* Wait until all the children are dead */
    for (int i = 0; i < numMonitors; i++) {
        wait(&monitors_pids[i]);
    }
	

}

void get_bloom_filters(HashTable monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd){

	printf("\n\n----------------GET-BLOOM-FILTERS-------------------\n\n");
    fd_set active, ready; /* represents file descriptor sets for the select function */
    FD_ZERO(&active); /* initializes the file descriptor set 'active' to be the empty set */

	/* Add file descriptors in read_fd array to the file descriptor set 'active' */
    for (int i = 0; i < numActiveMonitors; i++){
		FD_SET(read_fd[i], &active);
	}
	
	monitor m = NULL;
	HashTable monitor_viruses = NULL;
	virus_bloom v = NULL;
	char *virus_name = NULL, *bloom_filter = NULL;
    int counter = -1;

	/* For each of the active monitors, get the incoming bloom filters */
    while ((++counter) < numActiveMonitors){
        // Block until an input arrives from one of the workers

        /* Find out how many monitors are ready */
		ready = active;
        if (select(FD_SETSIZE, &(ready), NULL, NULL, NULL) < 0){
            perror("Error in select");
            exit(EXIT_FAILURE);
        }
        /* For all the monitors that have already sent bloom filters */
        for (int i = 0; i < numActiveMonitors; i++){
            if (FD_ISSET(read_fd[i], &ready)){

				/* Get the number of viruses the monitor reported */
				int n_viruses = atoi(receive_data(read_fd[i], bufferSize));
				printf("\nReported n_viruses: %d\n", n_viruses);
				printf("\n %s\n", receive_data(read_fd[i], bufferSize));
				

				/* Get the monitor with specified PID */
				m = HTSearch(monitors, &(monitors_pids[i]), compare_monitor);

				/* Read the bloom filters from the pipe */
				for (int j = 0; j < n_viruses; j++){
					virus_name = receive_data(read_fd[i], bufferSize);
					if (!strcmp(virus_name, "ready")) 
						break;
					printf("GBF-%s\n", virus_name);

					/* Get the hash table of viruses of monitor */
					monitor_viruses = get_monitor_viruses(m);

					/* Check if virus is already in hash table of viruses of monitor */
					/* If not, insert virus_bloom (v) in hash table of viruses of monitor */
					if ((v = HTSearch(monitor_viruses, virus_name, compare_virus_bloomName)) == NULL ){
						v = create_virus_bloom(virus_name, bloomSize);
						HTInsert(&(monitor_viruses), v, get_virus_bloomName);
					}

					bloom_filter = receive_data(read_fd[i], bufferSize);
					update_BloomFilter(v, bloom_filter);
					free(bloom_filter);
					virus_name = NULL;					
				}
				// while (true){
				// 	virus_name = receive_data(read_fd[i], bufferSize);
				// 	if (!strcmp(virus_name, "ready")) 
				// 		break;
				// 	fprintf(stderr, "GBF-%s\n", virus_name);

				// 	/* Get the hash table of viruses of monitor */
				// 	monitor_viruses = get_monitor_viruses(m);

				// 	/* Check if virus is already in hash table of viruses of monitor */
				// 	/* If not, insert virus_bloom (v) in hash table of viruses of monitor */
				// 	if ((v = HTSearch(monitor_viruses, virus_name, compare_virus_bloomName)) == NULL ){
				// 		v = create_virus_bloom(virus_name, bloomSize);
				// 		HTInsert(&(monitor_viruses), v, get_virus_bloomName);
				// 	}

				// 	bloom_filter = receive_data(read_fd[i], bufferSize);
				// 	update_BloomFilter(v, bloom_filter);
				// 	free(bloom_filter);
				// 	virus_name = NULL;
				// }
            }
        }
    }
}