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
#include "date.h"
#include "virus.h"

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
void run_queries(HashTable *monitors, int bufferSize, int *read_fd, int *write_fd);

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
    closedir(indir); free(input_dir);

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
		add_country(m, countries[country_idx]); /* add country in monitor to handle */
        send_data(write_fd[monitor_idx], bufferSize, countries[country_idx], 0); /* inform the child process through the pipe */
		
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
	run_queries(&monitors, bufferSize, read_fd, write_fd);

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

void get_bloom_filters(HashTable *monitors, pid_t *monitors_pids, int numActiveMonitors, int bufferSize, int bloomSize, int *read_fd){

    fd_set active, ready; /* represents file descriptor sets for the select function */
    FD_ZERO(&active); /* initializes the file descriptor set 'active' to be the empty set */

	/* Add file descriptors in read_fd array to the file descriptor set 'active' */
    for (int i = 0; i < numActiveMonitors; i++){
		FD_SET(read_fd[i], &active);
	}
	
	monitor m = NULL;
	virus_bloom v = NULL;
	char *virus_name = NULL, *bloom_filter = NULL;
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
			// fprintf(stderr, "\n-----------------------i=%d------------------------\n", i);
			/* Get the monitor with specified PID */
			m = HTSearch(*monitors, &(monitors_pids[i]), compare_monitor);

			/* Read the bloom filters from the pipe */
			while (true){
				virus_name = receive_data(read_fd[i], bufferSize);
				if (!strcmp(virus_name, "ready")){
					free(virus_name);
					break;
				}
				// fprintf(stderr, "GBF-%s", virus_name);

				/* Check if virus is already in hash table of viruses of monitor */
				/* If not, insert virus_bloom (v) in hash table of viruses of monitor */
				if ((v = HTSearch(get_monitor_viruses(m), virus_name, compare_virus_bloomName)) == NULL){
					v = create_virus_bloom(virus_name, bloomSize);
					add_virus(m, v);
				}
				// fprintf(stderr, " -AND- ");
				bloom_filter = receive_data(read_fd[i], bufferSize);
				// fprintf(stderr, "BLOOM-GBF\n");
				update_BloomFilter(v, bloom_filter);

				free(bloom_filter);
				free(virus_name);
			}
            counter++;
			FD_CLR(read_fd[i], &active);
        }
    }
}

void travelRequest(char *args[5], HashTable *monitors, int bufferSize, int *read_fd, int *write_fd){

	/* Get data from arguments with right order */
	char *id = args[0];
	char *str_date = args[1];
	char *countryFrom = args[2];
	// char *countryTo = args[3];
	char *virusName = args[4];

	/* Find the monitor that handles countryFrom */
	monitor m = NULL;
	virus_bloom v = NULL;
	List head = NULL;
	bool broke = false;
	for (int i = 0; i < HTSize(*monitors); i++){
		if ((head = get_HTchain(*monitors, i)) != NULL){
			for (ListNode node = list_first(head); node != NULL; node = list_next(head, node)){
				m = list_node_item(head, node);
				if (handles_country(m, countryFrom)){
					broke = true;
					break;
				}
			}
		}
		if (broke) break;
	}

	if (m == NULL){
		printf(RED "\nERROR: Given countryFrom not in database\n" RESET);
		return;
	}
	printf("Monitor PID: %d", *((int *)get_monitor_pid(m)));

	/* Check if virus is in hash table of viruses of monitor */
	if ((v = HTSearch(get_monitor_viruses(m), virusName, compare_virus_bloomName)) == NULL){
		printf(RED "\nERROR: Given virusName not in database\n" RESET);
		return;		
	}	

	/* Search in bloom filter of monitor */
	// if (!(BloomSearch(get_bloom(v), id))){
	// 	printf("bloom of parent\n");
	// 	printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
	// }
		
	// else{
		/* Create the query and write it to pipe */
		char *travelRequest = "/travelRequest";
		char query[strlen(travelRequest) + strlen(id) + strlen(virusName) + 3];
		snprintf(query, sizeof(query), "%s %s %s", travelRequest, id, virusName);

		int fd_index = get_fd_index(m);
		send_data(write_fd[fd_index], bufferSize, query, 0);

		char *response = receive_data(read_fd[fd_index], bufferSize);
		printf("\nresponse in parent: %s\n", response);
		if (!strcmp(response, "NO"))
			printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
		else{
			// char *yes = strtok(response, " ");
			// char *str_dateVaccinated = strtok(NULL, " \n");
			char *str_dateVaccinated = response + 5;
			date dateVaccinated = create_date(str_dateVaccinated);
			date dateTravel = create_date(str_date);
			if (date_between(dateVaccinated, dateTravel, six_months_ago(dateTravel)))
				printf("REQUEST ACCEPTED - HAPPY TRAVELS\n");
			else
				printf("REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
		}
		free(response);
	// }
}

void run_queries(HashTable *monitors, int bufferSize, int *read_fd, int *write_fd){
	/* Read input from stdin */
	char *line = NULL;
    size_t len = 0;
	// virus v = NULL;
	bool broke = false;

	printf(GRN "\nEnter command:\n" RESET);

	while (getline(&line, &len, stdin) != -1){

		char *query = strtok(line, " \n");

		if (strcmp(query, "/travelRequest") == 0){
			
			char *args[5];
			for (int i = 0; i < 5; i++){
				args[i] = strtok(NULL, " \n");

				if (args[i] == NULL){
					printf( RED "\nERROR: Invalid input\n" RESET
							YEL "Input format for this command: " RESET
							"/travelRequest citizenID date countryFrom countryTo virusName\n"
							GRN "\nEnter command:\n" RESET);
					broke = true;
					break;
				}
			}
			if (broke){
				broke = false;
				continue;
			}
			travelRequest(args, monitors, bufferSize, read_fd, write_fd);	
		}
		else if (strcmp(query, "/travelStats") == 0){

		}
		else if (strcmp(query, "/addVaccinationRecords") == 0){

		}
		else if (strcmp(query, "/searchVaccinationStatus") == 0){

		}
		else if (strcmp(query, "/exit") == 0)
			break;
		else
			printf( RED "\nERROR: Invalid input\n" RESET
				YEL "Input format for every command:\n" RESET
				"/travelRequest citizenID date countryFrom countryTo virusName\n"
				"/travelStats virusName [date1 date2] [country]\n"
				"/addVaccinationRecords country\n"
				"/searchVaccinationStatus citizenID\n"
				"/exit\n");			
		printf(GRN "\nEnter command:\n" RESET);
	}
	free(line);
}