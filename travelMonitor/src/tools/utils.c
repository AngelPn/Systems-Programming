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

void get_bloom_filters(HashTable *monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd);

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
        free(name1);
        free(name2);
    }

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
    closedir(indir);

	/* Sort countries array alphabetically using qsort() from the C standard library */
	qsort(countries, numSubdirs, sizeof(char *), cmpstr);

    /* Create a hash table to store countries RR alphabetically per monitor */
	HashTable monitors = HTCreate(Integer, destroy_monitor);
	int monitor_idx = 0, numActiveMonitors = 0;
	for (int country_idx = 0; country_idx < numSubdirs; country_idx++){
		monitor m = NULL;
		pid_t monitor_pid = monitors_pids[monitor_idx];

		/* Check if monitor with PID is already in hash table of monitors */
		/* If not, insert monitor (m) in hash table of monitors */
		if ((m = HTSearch(monitors, &monitor_pid, compare_monitor)) == NULL ){
			m = create_monitor(monitor_pid, monitor_idx);
			HTInsert(&(monitors), m, get_monitor_pid);
		}

		/* Assign the country subdir to monitor */
		char *countryName = countries[country_idx];
		add_country(m, countryName); /* add country in monitor to handle */
		
        send_data(write_fd[monitor_idx], bufferSize, countryName, 0); /* inform the child process through the pipe */
		
		if ((++monitor_idx) == numMonitors){
			monitor_idx = 0; /* reset monitor_idx */
			numActiveMonitors = 1; /* set numActiveMonitors to declare that all monitors are active */
		}
	}

    /* Write 'end' into every pipe to note the end of distribution of countries per monitor */
    for (int i = 0; i < numMonitors; i++){
		send_data(write_fd[i], bufferSize, "end", 0);
	}

	/* Update numActiveMonitors to declare the number of active monitors */
	numActiveMonitors = (numActiveMonitors == 0) ? monitor_idx : numMonitors;

    /* Get bloom filters from monitors */
	get_bloom_filters(&monitors, monitors_pids, numActiveMonitors, bufferSize, bloomSize, read_fd);

	/* Run queries */
	run_queries(&monitors, bufferSize, bloomSize, monitors_pids, read_fd, write_fd, numActiveMonitors, input_dir);

    /* Send a SIGKILL to the monitors to end them */
    for (int i = 0; i < numMonitors; i++) {
        kill(monitors_pids[i], SIGKILL);
    }
    /* Delete named pipes and remove tmp dir */
    for (int i = 0; i < numMonitors; i++) {
        unlink(names1[i]);
        unlink(names2[i]);
     
        free(names1[i]);
        free(names2[i]);
    }
	rmdir("./tmp");
	free(input_dir);
	
    /* Wait until all the children are dead */
    for (int i = 0; i < numMonitors; i++) {
        wait(&monitors_pids[i]);
    }

	/* Create the LogFiles dir with read/write/search permissions for owner, group and others */
    mkdir("./LogFiles", S_IRWXU | S_IRWXG | S_IRWXO);

	int accepted = 0, rejected = 0;
	monitor m = NULL;
	List head = NULL, m_countries = NULL;

	/* Traverse Hash Table of monitors */
	for (int i = 0; i < HTSize(monitors); i++){
		if ((head = get_HTchain(monitors, i)) != NULL){
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){

				/* Create the log_file.xxx for monitor with PID xxx and open it */
				m = list_node_item(head, node);
				char *filepath = concat_int_to_string("./LogFiles/log_file.", *((int *)get_monitor_pid(m)));
				FILE *logfile = NULL;
				if ((logfile = fopen(filepath, "w")) == NULL){
					perror(RED "Error opening Log file"  RESET);
					exit(EXIT_FAILURE);
				}

				/* Write to log_file.xxx the countries that monitor handles */
				m_countries = get_monitor_countries(m);
				for (ListNode n = list_first(m_countries); n != NULL; n = list_next(m_countries, n)){
					fprintf(logfile, "%s\n", (char *)list_node_item(m_countries, n));
				}

				/* Write to log_file.xxx the total number of requests */
				accepted = get_total_accepted(m);
				rejected = get_total_rejected(m);
				fprintf(logfile, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n", accepted+rejected, accepted, rejected);
				
				fclose(logfile);
				free(filepath);
				accepted = rejected = 0;
			}
		}
	}

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
	}
	/* Read the bloom filters the reborn child has sent */
	read_bloom_filters(i, reborn_m, bufferSize, bloomSize, read_fd);
}
